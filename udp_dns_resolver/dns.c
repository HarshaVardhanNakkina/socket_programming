#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

#include <netdb.h>
#include <arpa/inet.h>

#define PORT 5050
#define MAXLINE 1024


int main(int argc, char *argv[]) {
		
		char buffer[100];

		struct hostent *host_entry;
		struct in_addr **addr_list;

		struct sockaddr_in server_addr, client_addr;

		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin_family 			= AF_INET;
		server_addr.sin_port 				= htons(PORT);
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

		//create a udp server;
		int udp_fd;
		udp_fd = socket(AF_INET,	SOCK_DGRAM, 0);
		bind(udp_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));

		socklen_t len = sizeof(client_addr);
		bzero(buffer, sizeof(buffer));

		int n = recvfrom(udp_fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr, &len);

		puts(buffer);

		if ((host_entry = gethostbyname(buffer)) == NULL) {
			herror("gethostbyname");
			printf("could not resolve 🙏\n");
		} else {
			addr_list = (struct in_addr**) host_entry -> h_addr_list;
			int i;
			for (i = 0; addr_list[i] != NULL; i++) {
				bzero(buffer, sizeof(buffer));
				sprintf(buffer, "%s has resolved to %s \n", host_entry->h_name, inet_ntoa(*addr_list[i]));
				sendto(udp_fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
			}
		}

		sendto(udp_fd, "end", sizeof("end"), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
		bzero(buffer, sizeof(buffer));

		struct servent *serv_ent;
		recvfrom(udp_fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr, &len);
		puts(buffer);


		if( (serv_ent = getservbyname(buffer, NULL)) == NULL ) {
			herror("getservebyname");
			printf("could not resolve 🙏\n");
		} else {
			bzero(buffer, sizeof(buffer));
			sprintf(buffer, "%s: %d", serv_ent->s_name, htons(serv_ent->s_port)); 
			sendto(udp_fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
		}
		sendto(udp_fd, "end", sizeof("end"), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
		
		close(udp_fd);

		return 0;
}
