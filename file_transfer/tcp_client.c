#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 5050
#define MAXLINE 1024

void error(const char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char const *argv[]) {
  
  int client_fd;
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    printf("creation failed... :(");
    exit(0);
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if( connect(client_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0 )
    error("connection failed... :(");

  char buffer[1024];
  for(;;) {
    bzero(buffer, sizeof(buffer));
    fgets(buffer, sizeof(buffer), stdin);
    write(client_fd, buffer, sizeof(buffer));

    int file_count = 0;
    while( read(client_fd, buffer, sizeof(buffer)) > 0 ) {
      printf("%d %s\n", ++file_count, buffer);
      bzero(buffer, sizeof(buffer));
    }

    if((strncmp(buffer, "exit", 4) == 0)) {
      write(client_fd, "exit", sizeof(buffer));
      close(client_fd);
      exit(0);
    }
  }

  close(client_fd);

  return 0;
}
