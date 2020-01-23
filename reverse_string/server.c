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

#include <unistd.h>

#define PORT 5050
#define MAXLINE 1024


int main(int argc, char const *argv[]) {
	
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

	char buffer[MAXLINE];

	for(;;) {
		socklen_t len;
		struct sockaddr_in client_addr;
		len = sizeof(client_addr);

		int client_fd = accept(tcp_fd, (struct sockaddr*) &client_addr, &len);
		for(;;) {
			bzero(buffer, sizeof(buffer));
			printf("client: ");
			read(client_fd, buffer, sizeof(buffer));
			puts(buffer);

			if (strncmp(buffer, "exit", 4) == 0) {
				close(client_fd);
				exit(0);
			}

			int lo = 0, hi = strlen(buffer) - 1;
			
			while(lo < hi) {
				char temp = buffer[lo];
				buffer[lo] = buffer[hi];
				buffer[hi] = temp;
				lo++,
				hi--;
			}

			write(client_fd, buffer, sizeof(buffer));
			printf("you: ");
			puts(buffer);
		}

	}

	close(tcp_fd);

	return 0;
}
