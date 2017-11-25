/*
	https://www.zimuel.it/blog/sign-and-verify-a-file-using-openssl
	https://stackoverflow.com/questions/5140425/openssl-command-line-to-verify-the-signature
	https://stackoverflow.com/questions/10782826/digital-signature-for-a-file-using-openssl
	https://raymii.org/s/tutorials/Sign_and_verify_text_files_to_public_keys_via_the_OpenSSL_Command_Line.html
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

#define COMMAND_LEN_MAX 2048
#define FILE_NAME_MAX 100
#define COMMAND_BLOCKS_MAX 100
#define COMMAND_BLOCKS_LEN_MAX 50

#define TIME_ELAPSED(start, end) ((double) ((end) - (start))) / CLOCKS_PER_SEC

void execute(char *command);
void keygen(int keylen);
void sign(int sha, int rsa, char *file_name);
void verify(int sha, int rsa, char *file_name);
void cleanup(void);

char *passphrase = "sample_passphrase";
char *rsa_priv_1024 = "rsaprivatekey1024.pem";
char *rsa_pub_1024 = "rsapublickey1024.pem";
char *rsa_priv_2048 = "rsaprivatekey2048.pem";
char *rsa_pub_2048 = "rsapublickey2048.pem";

int main(void)
{
	clock_t start, end;
	
	printf ("##### Generating RSA Keys #####\n");

	keygen (1024);
	keygen (2048);

	printf ("Complete.\n\n");

	char *file_name;

	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir(".")) == NULL) {
		fprintf(stderr, "Could not open current directory.\n");

		return -1;
	}

	while ((ent = readdir (dir)) != NULL) {
		file_name = ent->d_name;

		if (strcmp (file_name, ".") == 0 || strcmp (file_name, "..") == 0)
			continue;

		printf ("##### Generating signatures for file: %s #####\n", file_name);

		sign (128, 1024, file_name);
		sign (256, 1024, file_name);
		sign (128, 2048, file_name);
		sign (256, 2048, file_name);

		printf ("Complete.\n\n");

		printf ("Verifying signatures\n");

		verify (128, 1024, file_name);
		verify (256, 1024, file_name);
		verify (128, 2048, file_name);
		verify (256, 2048, file_name);

		printf ("Complete.\n\n");
	}

	printf ("Cleanup.\n");
	cleanup();

	return 0;
}

void keygen(int keylen)
{
	char command[COMMAND_LEN_MAX];
	clock_t start, end;

	printf ("Generating RSA keys for key lenght: %d\n", keylen);

	char *rsa_priv = (keylen == 1024 ? rsa_priv_1024 : rsa_priv_2048);
	char *rsa_pub = (keylen == 1024 ? rsa_pub_1024 : rsa_pub_2048);

	sprintf(command, "openssl genrsa -aes128 -passout pass:%s -out %s %d &&\
		openssl rsa -in %s -outform PEM -pubout -passin pass:%s -out %s",
		passphrase, rsa_priv, keylen, rsa_priv, passphrase,
		rsa_pub);

	start = clock();
	system (command);
	end = clock();

	printf ("Done.\n");
	printf ("Time Elapsed: %lf sec\n", TIME_ELAPSED(start, end));
}

void sign(int sha, int rsa, char *file_name)
{
	char command[COMMAND_LEN_MAX];
	clock_t start, end;

	printf ("Creating signature for Keylen: %d and SHA%d\n", rsa, sha);

	sprintf(command, "openssl dgst -sha%d -passin pass:%s -sign %s -out sign_%d_%d %s",
		(sha == 128 ? 1 : 256), passphrase, (rsa == 1024 ? rsa_priv_1024 : rsa_priv_2048),
		sha, rsa, file_name);

	start = clock();
	system (command);
	end = clock();

	printf ("Done\n");
	printf ("Time Elapsed: %lf sec\n", TIME_ELAPSED(start, end));
}

void cleanup(void)
{
	char command[COMMAND_LEN_MAX];

	sprintf (command, "rm sign_* %s %s %s %s", rsa_priv_1024, rsa_pub_1024,
		rsa_priv_2048, rsa_pub_2048);

	system (command);
}

void verify(int sha, int rsa, char *file_name)
{
	char command[COMMAND_LEN_MAX];
	clock_t start, end;

	printf ("Verifying signature for Keylen: %d and SHA%d\n", rsa, (sha == 128 ? 1 : 256));

	sprintf(command, "openssl dgst -sha%d -verify %s -signature sign_%d_%d %s",
		(sha == 128 ? 1 : 256), (rsa == 1024 ? rsa_pub_1024 : rsa_pub_2048),
		sha, rsa, file_name);

	start = clock();
	system (command);
	end = clock();

	printf ("Done\n");
	printf ("Time Elapsed: %lf sec\n", TIME_ELAPSED(start, end));
}
