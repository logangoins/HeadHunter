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

int sendfile(FILE* fp, int fd, char* key)
{
	char data[SIZE] = {0};
	
	while(fgets(data, SIZE, fp) != NULL) {
		char* xordata = XOR(data, key, SIZE, strlen(key));
		if (send(fd, xordata, sizeof(data), 0) == -1) {
			exit(1);
		}
		bzero(data, SIZE);
		free(xordata);
	}
	char* eof = "--HEADHUNTER EOF--";
	char* xoreof = XOR(eof, key, strlen(eof), strlen(key));
	send(fd, xoreof, strlen(eof), 0);
	free(xoreof);
	//fclose(fp);
	return 0;
}

int recvfile(char* filename, int fd, char* key)
{
	int n;
 	FILE *fp;
	char buffer[SIZE];
	fp = fopen(filename, "w");
	while(1){
		n = recv(fd, buffer, SIZE, 0);
		if (n <= 0){
			break;
			return 0;
		}
		char* xorbuffer = XOR(buffer, key, n, strlen(key));
		if(str_starts_with(xorbuffer, "--HEADHUNTER EOF--") == 0){
			printf("[+] File successfully written to \"out.hunter\"\n");
			fclose(fp);
			return 0;
		}
		fprintf(fp, "%s", xorbuffer);
		bzero(buffer, SIZE);
		free(xorbuffer);
  		}
		

  return 0;
}
