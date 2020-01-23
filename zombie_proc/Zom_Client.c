#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
void str_cli(FILE *fp, int sockfd);

int main(int argc, char *argv[]) {
  int sockfd[5], con, i;
  struct sockaddr_in servaddr;
  if (argc != 2) {
    fprintf(stderr, "usage:ClientHostName\n");
    exit(1);
  }
  for (i = 0; i < 5; i++) {
    sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd[i] == -1)
      printf("Socket not created\n");
    else
      printf("Socket created\n");
    
    memset(&(servaddr.sin_zero), '\0', 8);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    con = connect(sockfd[i], (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (con == -1)
      printf("connection not established\n");
    else
      printf("Connection established\n");
  }
  str_cli(stdin, sockfd[0]);
  exit(0);
}
void str_cli(FILE *fp, int sockfd) {
  char sendline[500], recvline[500];
  while (fgets(sendline, 500, stdin) != NULL)
  {
    send(sockfd, sendline, strlen(sendline), 0);
    if (recv(sockfd, recvline, 500, 0) == 0)
      printf("Server terminated prematurely\n");
    fputs(recvline, stdout);
  }
}
