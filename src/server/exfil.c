#include <asm-generic/errno-base.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define SIZE 1024

int recvfile(char* filename, int fd, char* key)
{
	int n;
 	FILE *fp;
	char buffer[SIZE];
	
	fp = fopen(filename, "w");
	while((n = recv(fd, buffer, SIZE, 0)) > 0){
		char* xorbuffer = XOR(buffer, key, n, strlen(key));
		if(str_starts_with(xorbuffer, "--HEADHUNTER EOF--") == 0){
			printf("[+] File successfully written to \"out.hunter\"\nPress enter to return to previous session.\n");
			fclose(fp);
			return 0;
		}
		fprintf(fp, "%s", xorbuffer);
		bzero(buffer, SIZE);
		free(xorbuffer);
  	}
		
  fclose(fp);
  return 0;
}
