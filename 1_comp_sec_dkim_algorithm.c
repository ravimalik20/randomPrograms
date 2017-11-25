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

#define COMMAND_LEN_MAX 2048
#define COMMAND_BLOCKS_MAX 100
#define COMMAND_BLOCKS_LEN_MAX 50

void execute(char *command);

int main(void)
{
	char command_keygen_1024[COMMAND_LEN_MAX], command_keygen_2048[COMMAND_LEN_MAX];
	char *passphrase = "sample_passphrase";
	char *rsa_priv_1024 = "rsaprivatekey1024.pem";
	char *rsa_pub_1024 = "rsapublickey1024.pem";
	char *rsa_priv_2048 = "rsaprivatekey2048.pem";
	char *rsa_pub_2048 = "rsapublickey2048.pem";

	sprintf(command_keygen_1024, "openssl genrsa -aes128 -passout pass:%s -out %s 1024 &&\
		openssl rsa -in %s -outform PEM -pubout -passin pass:%s -out %s",
		passphrase, rsa_priv_1024, rsa_priv_1024, passphrase, rsa_pub_1024);

	sprintf(command_keygen_2048, "openssl genrsa -aes128 -passout pass:%s -out %s 2048 &&\
		openssl rsa -in %s -outform PEM -pubout -passin pass:%s -out %s",
		passphrase, rsa_priv_2048, rsa_priv_2048, passphrase, rsa_pub_2048);

	printf ("Generating RSA 1024 key pair.\n");
	system(command_keygen_1024);
	printf ("Complete\n");

	printf ("Generating RSA 1024 key pair.\n");
	system(command_keygen_2048);
	printf ("Complete\n");

	int i;
	char file_name[20];

	for (i = 1 ; i <= 10 ; i++) {
		sprintf(file_name, "msg%d.txt", i);

		printf ("Processing for file: %s\n", file_name);

		char command_sha_128_digest_1024[COMMAND_LEN_MAX];
		char command_sha_256_digest_1024[COMMAND_LEN_MAX];
		char command_sha_128_digest_2048[COMMAND_LEN_MAX];
		char command_sha_256_digest_2048[COMMAND_LEN_MAX];

		sprintf(command_sha_128_digest_1024, "openssl dgst -sha1 -passin pass:%s -sign %s -out sign_128_1024 %s",
			passphrase, rsa_priv_1024, file_name);

		sprintf(command_sha_256_digest_1024, "openssl dgst -sha256 -passin pass:%s -sign %s -out sign_256_1024 %s",
			passphrase, rsa_priv_1024, file_name);

		sprintf(command_sha_128_digest_2048, "openssl dgst -sha1 -passin pass:%s -sign %s -out sign_128_2048 %s",
			passphrase, rsa_priv_2048, file_name);

		sprintf(command_sha_256_digest_2048, "openssl dgst -sha256 -passin pass:%s -sign %s -out sign_256_2048 %s",
			passphrase, rsa_priv_2048, file_name);

		printf ("Generating signatures:\n");

		system(command_sha_128_digest_1024);
		system(command_sha_256_digest_1024);
		system(command_sha_128_digest_2048);
		system(command_sha_256_digest_2048);

		printf ("Complete.\n");

		char command_verify_sha_128_digest_1024[COMMAND_LEN_MAX];
		char command_verify_sha_256_digest_1024[COMMAND_LEN_MAX];
		char command_verify_sha_128_digest_2048[COMMAND_LEN_MAX];
		char command_verify_sha_256_digest_2048[COMMAND_LEN_MAX];

		sprintf(command_verify_sha_128_digest_1024, "openssl dgst -sha1 -verify %s -signature %s %s",
			rsa_pub_1024, "sign_128_1024", file_name);

		sprintf(command_verify_sha_256_digest_1024, "openssl dgst -sha256 -verify %s -signature %s %s",
			rsa_pub_1024, "sign_256_1024", file_name);

		sprintf(command_verify_sha_128_digest_2048, "openssl dgst -sha1 -verify %s -signature %s %s",
			rsa_pub_2048, "sign_128_2048", file_name);

		sprintf(command_verify_sha_256_digest_2048, "openssl dgst -sha256 -verify %s -signature %s %s",
			rsa_pub_2048, "sign_256_2048", file_name);

		printf ("Verifying signatures.\n");

		system(command_verify_sha_128_digest_1024);
		system(command_verify_sha_256_digest_1024);
		system(command_verify_sha_128_digest_2048);
		system(command_verify_sha_256_digest_2048);

		printf ("Complete\n");
	}

	return 0;
}

void execute(char *command)
{
	char *command_execve[COMMAND_BLOCKS_MAX];

	char cmd[100];
	strcpy(command, cmd);

	char *command_name;

	int i = 0;
	const char del[2] = " ";
	char *token = strtok (cmd, del);

	command_name = token;

    while (token != NULL) {
        token = strtok (NULL, del);

		command_execve[i++] = token;
    }

	execv(command_name, command_execve);
}
