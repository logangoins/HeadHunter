#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "server.h"

extern struct sockaddr_in cli;
extern socklen_t len;
extern int client_socket[MAX_CLIENTS];
extern args a;

char *get_socket_addr(int socket_descriptor){
    getpeername(socket_descriptor, (struct sockaddr *)&cli, &len);

    return inet_ntoa(cli.sin_addr);
}

int str_starts_with(char *a, char*b){
    for(int i = 0; i < strlen(b); i++){
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

int have_connections(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (client_socket[i] != 0){
            return 1;
        }
    }
    return 0;
}

int get_sockd_index(int sockd){
    for (int i = 0; i < MAX_CLIENTS; i++){
        // printf("{%d}%d\n", i, client_socket[i]);
        if (client_socket[i] == sockd){
            return i;
        }
    }
    return -1;
}

int extra_newline_eraser(char* buffer){

    for(int i = 0; i < MAXBUF - 1; i++){


        if(buffer[i] == '\n' && buffer[i+1] == '\n'){
            int e,j;
            e = 0;
            while(e<strlen(buffer)){
                if(buffer[e] == '\n'){

                    for(j = e; j < strlen(buffer) - 1; j++)

                        buffer[j] = buffer[j+1];
                } else e++;
            }

        }
    }

    return 0;
}

