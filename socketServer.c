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

int main(){
  
  serv.sin_family = AF_INET;
  serv.sin_port = htons(4444); //Listening port definition
  serv.sin_addr.s_addr = INADDR_ANY;

  fd = socket(AF_INET, SOCK_STREAM, 0); //Creates socket and returns identifier to fd

  
  if(fd < 0){
    printf("Error"); //used to test if the socket is already in use
    exit(0);
  }


  bind(fd, (struct sockaddr *)&serv, sizeof(serv)); //Assigns address to the socket

  listen(fd, 5); //Listen for client connections. Maximum 5 connections
  while(conn = accept(fd, (struct sockaddr *)NULL, NULL)) {
      int pid;
      if((pid = fork()) == 0){

        printf("Enter a message: ");
        
        fgets(message, 100, stdin);
        printf("Message: %s\n", message);
        fflush(stdout);
        send(conn, message, strlen(message), 0);
        printf("Hehe - michael jackson\n");
        while (recv(conn, message, 100, 0) > 0) {
          printf("Message Received: %s\n", message);
          strcpy(message, "");
        }
      }
    }
  
   return 0;
}
