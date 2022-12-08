// Client side C/C++ program to demonstrate Socket
// programming
#include <stdio.h>
#include <sys/socket.h> 
#include <stdlib.h>
#include <netinet/in.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

//Note the comments for variables in server code
int main(){

  struct sockaddr_in serv; 
  int fd; 
  int conn; 
  char message[100] = ""; 

  fd = socket(AF_INET, SOCK_STREAM, 0);

  serv.sin_family = AF_INET;
  serv.sin_port = htons(4444);

  inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr); //Binds client to localhost

  connect(fd, (struct sockaddr *)&serv, sizeof(serv)); //connects client to server

  while(conn = accept(fd, (struct sockaddr *)NULL, NULL)) {
    int pid;
    if((pid = fork()) == 0){
      while (recv(conn, message, 100, 0) > 0) {
        printf("Message Received: %s\n", message);
        strcpy(message, "");
      }
      exit(0);
    }
  }

  return 0;
}
