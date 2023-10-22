#include "server.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern struct sockaddr_in cli;
extern socklen_t len;
extern int client_socket[MAX_CLIENTS];
extern args a;

char* get_socket_addr(int socket_descriptor)
{
    getpeername(socket_descriptor, (struct sockaddr*)&cli, &len);

    return inet_ntoa(cli.sin_addr);
}

int str_starts_with(char* a, char* b)
{
    for (int i = 0; i < strlen(b); i++) {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

int have_connections()
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_socket[i] != 0) {
            return 1;
        }
    }
    return 0;
}

int get_sockd_index(int sockd)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        // printf("{%d}%d\n", i, client_socket[i]);
        if (client_socket[i] == sockd) {
            return i;
        }
    }
    return -1;
}

char* newline_terminator(char* buffer)
{
    for (int i = 0; i < MAXBUF; i++) {
        if (buffer[i] == '\n') {
            buffer[i + 1] = '\0';
            break;
        }
    }

    return buffer;
}

char* XOR(char* data, char* key, int datalen, int keylen)
{
    char* output = (char*)malloc(sizeof(char) * datalen);

    for (int i = 0; i < datalen; ++i) {
        output[i] = data[i] ^ key[i % keylen];
    }

    return output;
}
