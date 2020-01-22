#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <unistd.h>

#define PORT 5050
#define MAXLINE 1024

int main(int argc, char const *argv[]) {
	
	int sock_fd;
	char buffer[MAXLINE];

	struct sockaddr_in server_addr;

	if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("oops!!! socket creation failed...\n");
		exit(0);
	}

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	char* msg = "does harsha love prachi?";
	sendto(sock_fd, (const char*) msg, strlen(msg), 0, (const struct sockaddr*) &server_addr, sizeof(server_addr));

	printf("server: ");
	int len, n;
	n = recvfrom(sock_fd, (char*) buffer, MAXLINE, 0, (struct sockaddr*) &server_addr, &len);

	puts(buffer);
	close(sock_fd);
	return 0;
}
