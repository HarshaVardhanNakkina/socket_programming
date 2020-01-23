#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
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

  if (argc < 2) {
    printf("no hostname found in args \n");
    exit(0);
  }
  printf("%s %s", argv[1], argv[2]);
	
	int sock_fd;
	char buffer[MAXLINE];

	struct sockaddr_in server_addr;

	if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("oops!!! socket creation failed... :(\n");
		exit(0);
	}

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	strcpy(buffer, argv[1]);
	sendto(sock_fd, buffer, strlen(buffer), 0, (const struct sockaddr*) &server_addr, sizeof(server_addr));

	bzero(buffer, sizeof(buffer));
	int len, n;
	while( recvfrom(sock_fd, buffer, MAXLINE, 0, (struct sockaddr*) &server_addr, &len) > 0 ) {
		// int error = 0;
		// socklen_t length = sizeof (error);
		// printf("%d\n", getsockopt (sock_fd, SOL_SOCKET, SO_ERROR, &error, &length));
		puts(buffer);
		bzero(buffer, sizeof(buffer));
	}
	return 0;
}
