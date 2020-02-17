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

	int soc;
	soc = socket(AF_INET, SOCK_STREAM, 0);

	char fileName[10];

	char data[1024];
	FILE *fp;

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(8080);

	inet_pton(AF_INET, "127.0.0.1", &sock_addr.sin_addr);

	//bind(socket,(struct sockaddr *)&address,sizeof(address));

	connect(soc, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
	int size = 10;
	while (size == 10) {
		size = recv(soc, data, 10, 0);
		int i = 0;
		for (i = 0; i < size; i++)
			printf("%c", data[i]);
	}

	scanf("%s", fileName);

	send(soc, fileName, strlen(fileName) + 1, 0);

	fp = fopen("file.dec", "wb");

	sleep(2);

	size = 1;
	while (size == 1) {

		size = recv(soc, data, 1, 0);
		int i = 0;
		fputc(data[i], fp);
	}
	fclose(fp);

	char decCmd1[500] = "openssl aes-256-cbc -d -pbkdf2 -in file.dec -out ";
	char decCmd2[500] = " -k hymn_for_the_weekend";

	strcat(decCmd1, fileName);

	strcat(decCmd1, decCmd2);

	system(decCmd1);

	char catFile[25] = "cat ";
	strcat(catFile, fileName);
	system(catFile);
}
