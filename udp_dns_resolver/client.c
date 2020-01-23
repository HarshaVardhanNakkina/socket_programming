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

void sendMessage(int sock_fd, struct sockaddr_in server_addr, char* msg) {

	char buffer[MAXLINE];

	strcpy(buffer, msg);
	sendto(sock_fd, buffer, strlen(buffer), 0, (const struct sockaddr*) &server_addr, sizeof(server_addr));

	bzero(buffer, sizeof(buffer));
	int len, n;
	while( recvfrom(sock_fd, buffer, MAXLINE, 0, (struct sockaddr*) &server_addr, &len) > 0 ) {
		if( (strncmp(buffer, "end", 3)) == 0 ) break;
		printf("%s\n", buffer);
		bzero(buffer, sizeof(buffer));
	}

}

int main(int argc, char *argv[]) {

  if (argc < 3) {
    printf("no hostname found in args \n");
    exit(0);
  }
  // printf("%s %s\n", argv[1], argv[2]);	
	
	int sock_fd;

	struct sockaddr_in server_addr;

	if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("oops!!! socket creation failed... :(\n");
		exit(0);
	}

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	sendMessage(sock_fd, server_addr, argv[1]);
	sendMessage(sock_fd, server_addr, argv[2]);

	
	return 0;
}
