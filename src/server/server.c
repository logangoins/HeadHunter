#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include "server.h"

#include "helpers.c"
#include "command_session.c"

int CreateServerSocket(char *address, char *port, int *type, int *family)
{
    int sockfd, gai_error;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = *family;   // passes parameters called into a struct
    hints.ai_socktype = *type;
    hints.ai_flags = AI_PASSIVE;

    if ((gai_error = getaddrinfo(address, port, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "Error in getaddrinfo(): %s\n", gai_strerror(gai_error));
        exit(0);
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (sockfd == -1)
        {
            perror("Error in socket()");
            continue;
        }

        if (*type == SOCK_STREAM)
        {
            int sockopt = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

            if (sockopt == -1)
            {
                printf("Error in function setsockopt\n");
            }
        }

        // binds socket
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("Error in bind()");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        printf("Error could not bind\n");
        exit(0);
    }

    // starts listening for incoming connections
    if (*type == SOCK_STREAM && listen(sockfd, BACKLOG) == -1)
    {
        printf("Error in function listen\n");
    }

    freeaddrinfo(servinfo);
    printf("Entering server command session\n");
    printf("Listener started... awaiting connection\nType \"help\" to see command list\n\n");
    return sockfd;
}

void Server(char *address, char *port, int *type, int *family) {
    pthread_t socket_reader;
    pthread_t socket_writer;
    pthread_t acceptor;
    int action;

    // calls function to create server socket then returns server file descriptor
    master_socket = CreateServerSocket(address, port, type, family);

    // create an acceptor child process to accept incoming connections
    if (pthread_create(&acceptor, NULL, Acceptor, NULL) != 0)
        printf("Error in function pthread_create\n");

    // Passes arguments to the thread struct
    a.src = STDIN_FILENO;
    a.dest = 0;

    while(1){
        action = server_control_session();

        if (action == -1){break;}  // Exit cleanly
        a.dest = action;
        a.kill = 0;

        if (pthread_create(&socket_reader, NULL, Socket_Reader, NULL) != 0)
            printf("Error in function pthread_create\n");
        if (pthread_create(&socket_writer, NULL, Socket_Writer, NULL) != 0)
            printf("Error in function pthread_create\n");

        // Kill socket reader/writer threads and WAIT for them to close
        pthread_join(socket_writer, NULL);
        if (a.kill == 1){
            pthread_cancel(socket_reader);
            pthread_cancel(socket_writer);
        }
    }

    close(master_socket);
}
