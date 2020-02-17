#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>


#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#define PORT 5050
#define MAXLINE 1024

int encrypt(unsigned char*, int, unsigned char*, unsigned char*, unsigned char*);
int decrypt(unsigned char*, int, unsigned char*, unsigned char*, unsigned char*);

void handleErrors(void) {
  ERR_print_errors_fp(stderr);
  abort();
}


int main(int argc, char const *argv[]) {

	// enc and dec related
  unsigned char *key = (unsigned char *) "012345678901234567890123456789";
  unsigned char *iv = (unsigned char *) "0123456789012345";
	
	int sock_fd;
	char buffer[MAXLINE];

	struct sockaddr_in server_addr;

	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket creation failed... \n");
		exit(0);
	}

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
		printf("connection failed...\n");
	}

	// unsigned char *plaintext = (unsigned char *) "the quick brown fox jumpes over a lazy dog";

  unsigned char ciphertext[MAXLINE], decryptedtext[MAXLINE];

  int decryptedtext_len, ciphertext_len;

	for(;;) {
		bzero(buffer, sizeof(buffer));
		fgets(buffer, MAXLINE, stdin);

		if (strncmp(buffer, "exit", 4) == 0) {
			close(sock_fd);
			exit(0);
		}
		bzero(ciphertext, sizeof(ciphertext));
		ciphertext_len = encrypt(buffer, strlen((char*) buffer), key, iv, ciphertext);
		printf("you: ");
		puts(buffer);
		write(sock_fd, ciphertext, sizeof(buffer));

		bzero(buffer, sizeof(buffer));
		printf("server: ");
		read(sock_fd, buffer, sizeof(buffer));

		bzero(decryptedtext, sizeof(decryptedtext));
		decryptedtext_len = decrypt(buffer, strlen((char*) buffer), key, iv, decryptedtext);
		decryptedtext[decryptedtext_len] = '\0';

		puts(decryptedtext);
	}

	close(sock_fd);
	return 0;
}

int encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* iv, unsigned char* ciphertext) {


  EVP_CIPHER_CTX *ctx;

  int len, ciphertext_len;

  if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    handleErrors();

  if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
  
  ciphertext_len = len;

  if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
    handleErrors();

  ciphertext_len += len;

  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext) {

  EVP_CIPHER_CTX *ctx;

  int len, plaintext_len;

  if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    handleErrors();

  if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    handleErrors();

  plaintext_len = len;

  if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
    handleErrors();

  plaintext_len += len;

  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}