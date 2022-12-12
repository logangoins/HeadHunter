// Recruiter Command and Control Server (C2) and payload generator
// Author: Logan Goins
//
// I am not liable for any malicious or illegal action
// that any user commits using this software.
// Include generate functionality, per platform
// allow payload to send commands back



#include "include/socketServer.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>


int main(int argc, char **argv){


  printf(R"EOF(
 
__________                          .__  __                
\______   \ ____   ___________ __ __|__|/  |_  ___________ 
 |       _// __ \_/ ___\_  __ \  |  \  \   __\/ __ \_  __ \
 |    |   \  ___/\  \___|  | \/  |  /  ||  | \  ___/|  | \/
 |____|_  /\___  >\___  >__|  |____/|__||__|  \___  >__|   
        \/     \/     \/                          \/       

  Command and Control Server (C2) and Payload Generator
  Author: Logan Goins


)EOF");


  if(argc <= 1){
    printf("No command specified, type \"recruiter --help\" for a list of commands\n\n");
  }

  else if(argc > 0 && strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
  printf("\nCommands\n--------------------------------------\n");
  printf("-h, --help                                      displays this help menu\n");
  printf("-l, --listen <port>                             starts a listening C2 server on a specified port\n");
  printf("-g, --generate <platform> -o <outputfile>       generates a reverse shell payload to connect back to a recruiter session\n\n");
  }  

  else if(argc > 1 && strcmp(argv[1], "--listen") == 0 || strcmp(argv[1], "-l") == 0){
    if (argc < 3){
      printf("Please supply a port for the server to listen on\n\n");
    } 
    else{
    int port = (int) strtol(argv[2], (char **)NULL, 10);
    startserv(port);
    }
  }

  else if(argc >= 5 && strcmp(argv[1], "--generate") == 0 || strcmp(argv[1], "-g") == 0){
    printf("Generate good times comeon\n");
  }

  else {
    printf("Invalid command, type \"recruiter --help\" for help");
  }

  return 0;  
}
