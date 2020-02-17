#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <errno.h>
#include <unistd.h>

#include <dirent.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
  struct sockaddr_in sock_addr;
  FILE *f;
  char buffer[1024];
  char fileName[1024];

  int serverfd, addrlen, newsocket, test;

  serverfd = socket(AF_INET, SOCK_STREAM, 0);

  sock_addr.sin_family = AF_INET;
  sock_addr.sin_addr.s_addr = INADDR_ANY;
  sock_addr.sin_port = htons(8080);

  bind(serverfd, (struct sockaddr *)&sock_addr, sizeof(sock_addr));

  printf("I am listining\n");

  listen(serverfd, 100);

  newsocket = accept(serverfd, (struct sockaddr *)&sock_addr, (socklen_t *)&addrlen);

  printf("Got Connection\n");

  system("ls ./files > file.txt");

  f = fopen("file.txt", "r");

  int counter = 0;
  while (!feof(f)) {
    char c = fgetc(f);
    buffer[counter++] = c;
  }

  fclose(f);

  buffer[counter - 1] = '\0';

  strcat(buffer, "\n\nPlease Enter the file to send\n");

  send(newsocket, buffer, strlen(buffer), 0);

  int bytesread = read(newsocket, fileName, 1024);

  char folderPath[500] = "./files/";

  fileName[bytesread - 1] = '\0';

  char encCmd1[500] = "openssl aes-256-cbc -e -pbkdf2 -in ";
  char encCmd2[500] = " -out file.enc -k hymn_for_the_weekend";

  strcat(folderPath, fileName);

  strcat(encCmd1, folderPath);

  strcat(encCmd1, encCmd2);

  system(encCmd1);

  f = fopen("file.enc", "rb");
  counter = 0;

  while (!feof(f)) {
    char c = fgetc(f);
    send(newsocket, &c, sizeof(c), 0);
  }

  fclose(f);

  shutdown(serverfd, SHUT_RDWR);
}
