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
  struct sockaddr_in bindaddress, oppaddress;
  char filedata[1024];
  char recvdata[1024];
  char fileName[200];
  char send[5] = "Hello";
  int sock, i;
  sock = socket(AF_INET, SOCK_DGRAM, 0);

  bindaddress.sin_port = htons(8070);
  bindaddress.sin_family = AF_INET;
  inet_pton(AF_INET, "127.0.0.1", &bindaddress.sin_addr);

  bind(sock, (struct sockaddr *)&bindaddress, sizeof(bindaddress));

  oppaddress.sin_port = htons(8080);
  oppaddress.sin_family = AF_INET;

  bzero(&oppaddress.sin_addr, sizeof(oppaddress.sin_addr));

  inet_pton(AF_INET, "127.0.0.1", &oppaddress.sin_addr);

  sendto(sock, send, 5, 0, (struct sockaddr *)&oppaddress, sizeof(oppaddress));

  int addrsize = sizeof(oppaddress);
  int dataReceived = recvfrom(sock, recvdata, 1024, 0, (struct sockaddr *)&oppaddress, (socklen_t *)&addrsize);

  recvdata[dataReceived] = '\0';

  printf("%s", recvdata);

  printf("Please Enter the fileName which you want to receive\n");

  scanf("%s", fileName);

  sendto(sock, fileName, strlen(fileName), 0, (struct sockaddr *)&oppaddress, sizeof(oppaddress));

  FILE *fp = fopen("file.dec", "wb");

  sleep(2);

  do {

    dataReceived = recvfrom(sock, recvdata, 10, 0, (struct sockaddr *)&oppaddress, (socklen_t *)&addrsize);
    for (i = 0; i < dataReceived; i++)
    {
      fputc(recvdata[i], fp);
    }
  } while (dataReceived >= 10);

  fclose(fp);

  char encCmd1[500] = "openssl aes-256-cbc -d -pbkdf2 -in file.dec -out ";
  char encCmd2[500] = " -k hymn_for_the_weekend";

  strcat(encCmd1, fileName);

  strcat(encCmd1, encCmd2);

  system(encCmd1);

  char catFile[25] = "cat ";
	strcat(catFile, fileName);
	system(catFile);
}
