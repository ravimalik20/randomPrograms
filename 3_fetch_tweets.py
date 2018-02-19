import json
from twitter import Twitter, OAuth, TwitterHTTPError, TwitterStream
from threading import Thread, Condition

ACCESS_TOKEN = '961360036525113345-Sh9wqziTRPqHnQvBAFqR6X7Ie2W9eV1'
ACCESS_SECRET = 'y1fluOM1sp6pJ4Wyc7L3pDwkEYByIQZW7xcCgZj23QJIf'
CONSUMER_KEY = 'hDl3Na9MVye4ZhVQXrtkakjNM'
CONSUMER_SECRET = 'c4os7vhVH33UYNYgt6mHAcbrSECAR8ZZugFKeyRmf3pFriXbjb'

class TweetFetchWorker(Thread):
	TWEETS_PER_REQUEST = 99
	MAX_TWEETS = 20000

	def __init__(self, access_token, access_secret, consumer_key,
		consumer_secret, query):
		Thread.__init__(self)

		self.access_token = access_token
		self.access_secret = access_secret
		self.consumer_key = consumer_key
		self.consumer_secret = consumer_secret

		self. oauth = OAuth(access_token, access_secret, consumer_key,
			consumer_secret)
		self.twitter = Twitter(auth = self.oauth)

		self.query = query

		self.tweets = dict()

	def run(self):
		try:
			wait = Condition()

			while len(self.tweets) <= TweetFetchWorker.MAX_TWEETS:
				with wait:
					if len(self.tweets.keys()) > 0:
						data = self.twitter.search.tweets(q = self.query,
							result_type='recent', lang = 'en',
							count = TweetFetchWorker.TWEETS_PER_REQUEST,
							max_id = min(self.tweets.keys())
						)
					else:
						data = self.twitter.search.tweets(q = self.query,
							result_type='recent', lang = 'en',
							count = TweetFetchWorker.TWEETS_PER_REQUEST
						)

					for tweet in data['statuses']:
						obj = {
							'id': tweet['id'],
							'text': tweet['text'],
							'geo': tweet['geo'],
							'coordinates': tweet['coordinates'],
							'created_at': tweet['created_at'],
							'location': tweet['user']['location'],
						}

						self.tweets[tweet['id']] = obj

					wait.wait(2)

		finally:
			final_data = list(self.tweets.values())
			f = open("tweets.json", "w")

			f.write(json.dumps(final_data, sort_keys = True, indent = 4))
			f.close()

			print ("Count: ", len(self.tweets))

if __name__ == "__main__":
	fetch = TweetFetchWorker(ACCESS_TOKEN, ACCESS_SECRET, CONSUMER_KEY,
		CONSUMER_SECRET, "#flu")
	fetch.start()
