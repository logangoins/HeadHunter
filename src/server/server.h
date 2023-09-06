#define MAXBUF 65536
#define BACKLOG 1
#define MAX_CLIENTS 30
#define PROMPT "headhunter/> "

fd_set readfds;
int master_socket, new_socket, max_clients = MAX_CLIENTS, client_socket[MAX_CLIENTS], activity, i, sd, victim_count;
int max_sd;
struct sockaddr_in cli;
socklen_t len;

// Declaration of struct "args" as the pthread_create function preferably takes a struct as a param
// This is the struct for the Writer thread
typedef struct args
{
    int src;
    int dest;
    int kill;
} args;

args a;