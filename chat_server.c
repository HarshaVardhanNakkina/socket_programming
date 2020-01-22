#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

void error(const char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char const *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "port number is not provided. Terminating...\n\n");
		exit(1);
	}

	int server_socket, portno = atoi(argv[1]);
	char buffer[256];
	struct sockaddr_in server_addr, client_addr;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(server_socket < 0) error("Error opening socket");

	bzero((char *) &server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portno);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) error("Error binding...");

	listen(server_socket, 5);

	socklen_t client_len = sizeof(client_addr);

	int client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_len);

	if(client_socket < 0) error("Error accepting the client...");

	while(1) {
		bzero(buffer, 256);
		printf("you: ");
		fgets(buffer, 256, stdin);

		if (write(client_socket, buffer, strlen(buffer)) < 0)
			error("Error writing to client...");

		bzero(buffer, 256);

		if (read(client_socket, buffer, 256) < 0)
			error("Error reading from cleint...");
		
		printf("client: %s \n", buffer);

		if (strncmp("Bye", buffer, 3) == 0) break;
	}

	// close(client_socket);
	// close the socket
	int shutdown_status = shutdown(client_socket, 2);
	if(shutdown_status)
		perror("error closing\n\n");
	else
		printf("connection closed\n\n");
	// close(server_socket);
	int shutdown_status = shutdown(server_socket, 2);
	if(shutdown_status)
		perror("error closing\n\n");
	else
		printf("connection closed\n\n");

	return 0;
}
