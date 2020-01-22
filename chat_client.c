#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>

void error(const char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char const *argv[]) {
	
	int client_socket, portno = atoi(argv[2]), n;
	struct sockaddr_in server_addr;
	struct hostent *server;

	char buffer[256];

	if(argc < 3) {
		fprintf(stderr, "usage %s [hostaddr] [portno]", argv[0]);
		exit(1);
	}

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(client_socket < 0)
		error("Error creating socket");

	server = gethostbyname(argv[1]);

	if(server == NULL)
		fprintf(stderr, "Error, no such host");

	bzero((char *) &server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portno);
	bcopy((char *) server->h_addr, (char *) &server_addr.sin_addr.s_addr, server->h_length);

	if(connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) error("Connection failed...");

	while(1) {
		bzero(buffer, 256);
		printf("you: ");
		fgets(buffer, 256, stdin);

		if(write(client_socket, buffer, strlen(buffer)) < 0)
			error("Error writing to server...");

		bzero(buffer, 256);

		if(read(client_socket, buffer, 256) < 0)
			error("Error reading from server...");

		printf("server: %s \n", buffer);

		if (strncmp("Bye", buffer, 3) == 0) break;
	}

	// close the socket
	int shutdown_status = shutdown(client_socket, 2);
	if(shutdown_status)
		perror("error closing\n\n");
	else
		printf("connection closed\n\n");

	return 0;
}
