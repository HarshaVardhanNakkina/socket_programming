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

int max(int x, int y) {
	if (x > y) return x;
	return y;
}

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
			// if ((child_pid = fork()) == 0) {
			// 	close(tcp_fd);
			// 	bzero(buffer, sizeof(buffer));
			// 	printf("tcp_client: ");
			// 	read(client_fd, buffer, sizeof(buffer));
			// 	printf("\nyou: ");
			// 	puts(buffer);
			// 	write(client_fd, "charan loves pallabi", sizeof(buffer));
			// 	close(client_fd);
			// 	exit(0);
			// }
			// close(client_fd);
				for(;;) {
					bzero(buffer, sizeof(buffer));
					printf("tcp_client: ");
					read(client_fd, buffer, sizeof(buffer));
					puts(buffer);

					if (strncmp(buffer, "exit", 4) == 0) {
						close(client_fd);
						exit(0);
					}

					int i = 0;
					
					while(buffer[i]) {
						int char_code = (int) buffer[i];
						if(char_code >= 97 && char_code <= 122)
							buffer[i] = buffer[i] - 32;
						i++;
					}
					buffer[i] = '\0';

					write(client_fd, buffer, sizeof(buffer));
					printf("you: ");
					puts(buffer);
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
			sendto(udp_fd, "harsha loves prachi but prachi loves sajal :(", sizeof(buffer), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
		}

	}

	close(tcp_fd);
	close(udp_fd);

	return 0;
}
