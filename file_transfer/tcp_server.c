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

int main(int argc, char const *argv[]) {
  
  int server_fd;
  struct sockaddr_in server_addr, client_addr;

  // create a socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    printf("creation failed... :(");
    exit(0);
  }

  // config the port and address
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET,
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr =  htonl(INADDR_ANY);

  // bind it to the address
  bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));

  // start listening
  listen(server_fd, 10);
  
  // accept incoming connection
  int len = sizeof(client_addr);
  int client_fd = accept(server_fd, (struct sockaddr*) &client_addr, &len);

  // get incoming messages
  char buffer[MAXLINE];
  for (;;) {
    bzero(buffer, sizeof(buffer));
    read(client_fd, buffer, sizeof(buffer));
    puts(buffer);

    if((strncmp(buffer, "exit", 4) == 0)) {
      close(client_fd);
      close(server_fd);
      exit(0);
    }

  }


  close(server_fd);
  return 0;
}
