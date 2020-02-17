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
	char recvData[1024];
	char fileName[200];
	char folderPath[200] = "./files/";
	int sock, i;
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	bindaddress.sin_port = htons(8080);
	bindaddress.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &bindaddress.sin_addr);

	bind(sock, (struct sockaddr *)&bindaddress, sizeof(bindaddress));

	system("ls ./files > file.txt");

	FILE *file = fopen("file.txt", "r");

	int counter = 0;
	while (!feof(file)) {
		filedata[counter++] = fgetc(file);
	}

	filedata[counter - 1] = '\0';

	int addrsize = sizeof(oppaddress);
	recvfrom(sock, recvData, 1024, 0, (struct sockaddr *)&oppaddress, (socklen_t *)&addrsize);

	printf("%s", filedata);

	oppaddress.sin_port = htons(8070);
	oppaddress.sin_family = AF_INET;

	bzero(&oppaddress.sin_addr, sizeof(oppaddress.sin_addr));

	sendto(sock, filedata, strlen(filedata), 0, (struct sockaddr *)&oppaddress, sizeof(oppaddress));

	int recvDataSize = recvfrom(sock, recvData, 1024, 0, (struct sockaddr *)&oppaddress, (socklen_t *)&addrsize);

	recvData[recvDataSize] = '\0';

	FILE *fp;

	strcat(folderPath, recvData);

	char encCmd1[500] = "openssl aes-256-cbc -e -pbkdf2 -in ";
	char encCmd2[500] = " -out file.enc -k hymn_for_the_weekend";

	strcat(encCmd1, folderPath);

	strcat(encCmd1, encCmd2);

	system(encCmd1);

	fp = fopen("file.enc", "r");

	while (!feof(fp)) {
		for (i = 0; i < 10 && !feof(fp); i++) {
			filedata[i] = fgetc(fp);
		}
		sendto(sock, filedata, i--, 0, (struct sockaddr *)&oppaddress, sizeof(oppaddress));
	}
}
