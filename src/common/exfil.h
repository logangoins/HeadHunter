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

int sendfile(char* file, int sfd){
	char* sendbuffer[100];
	int b;
	char* errbuff = "Error opening file...\n";  

	FILE *fp = fopen(file, "rb");
	if(fp == NULL{
		write(sfd, errbuff, sizeof(errbuff); 
	}    return 2;

	while( (b = fread(sendbuffer, 1, sizeof(sendbuffer), fp))>0 ){
		send(sfd, sendbuffer, b, 0);
	}

	fclose(fp);

	return 0;
	}

int recvfile(char* file, int rfd){
	FILE* fp = fopen(file , "wb");
	int tot = 0;
	char buff[1025];
	int b;

	if(fp != NULL){
	while( (b = recv(rfd, buff, 1024,0))> 0 ) {
		tot+=b;
		fwrite(buff, 1, b, fp);
	}

	printf("Received byte: %d\n",tot);
	if (b<0)
		perror("Error receiving file...\n");

	fclose(fp);
	} 
	else {
		perror("Error Receiving file...\n");
	}

	return 0;
}
