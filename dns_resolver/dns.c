/*

To develop a Client that contacts a given DNS Server (name to ip address mapping is present in
dns.txt) to resolve a given host name. (Note: client refer dns.txt locally before request sent to DNS
server)

*/

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <arpa/inet.h>


int main(int argc, char const *argv[]) {
	FILE *fd;
	fd = fopen("sites.txt", "r");

	char buffer[25];

	while (!feof(fd)) {
		int counter = 0;
		memset(buffer, 0, strlen(buffer));
		char c;
		while ((c = fgetc(fd)) != '\n' && !feof(fd))
			buffer[counter++] = c;
		buffer[counter] = '\0';

		struct hostent *host_entry;
		struct in_addr **addr_list;


		if ((host_entry = gethostbyname(buffer)) == NULL) {
			herror("gethostbyname");
			printf("could not resolve 🙏\n");
		} else {
			addr_list = (struct in_addr**) host_entry -> h_addr_list;
			int i;
			for (i = 0; addr_list[i] != NULL; i++)
				printf("%s has resolved to %s \n", host_entry->h_name, inet_ntoa(*addr_list[i]));
		}

	}

	fclose(fd);

	return 0;
}
