#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <unistd.h>

#define PORT 5050
#define MAXLINE 1024
/* 32 byte key (256 bit key) */
#define AES_256_KEY_SIZE 32
/* 16 byte block size (128 bits) */
#define AES_BLOCK_SIZE 16

int max(int x, int y) {
	if (x > y) return x;
	return y;
}

int encrypt(unsigned char*, int, unsigned char*, unsigned char*, unsigned char*);
int decrypt(unsigned char*, int, unsigned char*, unsigned char*, unsigned char*);

void handleErrors(void) {
  ERR_print_errors_fp(stderr);
  abort();
}

int main(int argc, char const *argv[]) {

	// encryption and decryption related
  unsigned char *key = (unsigned char *) "012345678901234567890123456789";
  unsigned char *iv = (unsigned char *) "0123456789012345";
		/* Key to use for encrpytion and decryption */
	// unsigned char key[AES_256_KEY_SIZE];

	/* Initialization Vector */
	// unsigned char iv[AES_BLOCK_SIZE];

	// RAND_bytes(key, sizeof(key));
	// RAND_bytes(iv, sizeof(iv));

  // unsigned char *plaintext = (unsigned char *) "the quick brown fox jumpes over a lazy dog";

  unsigned char ciphertext[MAXLINE], decryptedtext[MAXLINE];

  int decryptedtext_len, ciphertext_len;

	
	// create a tcp sever
	int tcp_fd;
	tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family 			= AF_INET;
	server_addr.sin_port 				= htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(tcp_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
	listen(tcp_fd, 10);

	//create a udp server;
	int udp_fd;
	udp_fd = socket(AF_INET,	SOCK_DGRAM, 0);
	bind(udp_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));

	fd_set read_set; // readset when kernel is tested for reading
	FD_ZERO(&read_set);

	int max_fd = max(tcp_fd, udp_fd) + 1; // max num of discr. ready

	char buffer[MAXLINE];

	for(;;) {
		FD_SET(tcp_fd, &read_set);
		FD_SET(udp_fd, &read_set);

		socklen_t len;
		struct sockaddr_in client_addr;

		/*
			int select(int maxfd, fd_set *readsset, fd_set *writeset,
			fd_set *exceptset, const struct timeval *timeout);
			Returns: positive count of descriptors ready, 0 on timeout, -1 on
			error
		*/

		int ready = select(max_fd, &read_set,NULL, NULL, NULL);

		// if TCP client is ready, then accept
		if (FD_ISSET(tcp_fd, &read_set)) {
			len = sizeof(client_addr);

			pid_t child_pid;

			int client_fd = accept(tcp_fd, (struct sockaddr*) &client_addr, &len);
				for(;;) {
					bzero(buffer, sizeof(buffer));
					printf("tcp_client: ");
					read(client_fd, buffer, sizeof(buffer));

					bzero(decryptedtext, sizeof(decryptedtext));
					decryptedtext_len = decrypt(buffer, strlen((char*) buffer), key, iv, decryptedtext);
					decryptedtext[decryptedtext_len] = '\0';

					puts(decryptedtext);

					if (strncmp(decryptedtext, "exit", 4) == 0) {
						close(client_fd);
						exit(0);
					}

					int i = 0;
					while(decryptedtext[i]) {
						int char_code = (int) decryptedtext[i];
						if(char_code >= 97 && char_code <= 122)
							decryptedtext[i] = decryptedtext[i] - 32;
						i++;
					}
					decryptedtext[i] = '\0';

					bzero(ciphertext, sizeof(ciphertext));
					ciphertext_len = encrypt(decryptedtext, strlen((char*) decryptedtext), key, iv, ciphertext);

					write(client_fd, ciphertext, sizeof(ciphertext));
					printf("you: ");
					puts(decryptedtext);
				}
				close(client_fd);
		}

		// if UDP client is ready, then receive
		if (FD_ISSET(udp_fd, &read_set)) {
			len = sizeof(client_addr);
			bzero(buffer, sizeof(buffer));
			printf("\n udp_client: ");

			int n = recvfrom(udp_fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr, &len);

			puts(buffer);
			sendto(udp_fd, "msg msg msg", sizeof(buffer), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
		}

	}

	close(tcp_fd);
	close(udp_fd);

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
