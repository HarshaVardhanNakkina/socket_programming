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
#define LISTENQ 10
void str_echo(int sockfd);
void sig_chld(int signo);
int main(int argc, char *argv[]) {
  int listenfd, connfd, bin, lis;
  pid_t childpid;
  socklen_t clilen;
  struct sockaddr_in cliaddr, servaddr;
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(8080);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd == -1)
    printf("Socket not created\n");
  else
    printf("Socket created\n");
  bin = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  if (bin == -1)
    printf("Not bind\n");
  else
    printf("Bind success\n");
  lis = listen(listenfd, LISTENQ);
  if (lis == -1)
    printf("Not listening\n");
  else
    printf("Listening\n");
  signal(SIGCHLD, sig_chld);
  for (;;) {

    clilen = sizeof(cliaddr);
    if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0) {
      if (errno == EINTR)
        continue;
      else
        printf("Accept error\n");
    }
    if ((childpid = fork()) == 0) {
      close(listenfd);
      str_echo(connfd);
      exit(0);
    }
    close(connfd);
  }
}
void str_echo(int sockfd) {
  ssize_t n;
  char line[500];
  for (;;) {
    if ((n = recv(sockfd, line, 500, 0)) == 0)
      return;
    send(sockfd, line, n, 0);
  }
}
void sig_chld(int signo) {
  pid_t pid;
  int stat;
  while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    printf("Child %d terminated\n", pid);
  return;
}
