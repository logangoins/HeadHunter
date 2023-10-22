#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "exfil.h"

#define SEND_BUFSIZE 100
#define RECV_BUFSIZE 1024

int sendfile(const char* filename, const int fd)
{
    char* sendbuffer[SEND_BUFSIZE];
    int byte;
    const char* errbuff = "Error opening file...\n";

    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        write(fd, errbuff, strlen(errbuff));
        return EIO;
    }

    while ((byte = fread(sendbuffer, 1, sizeof(sendbuffer), fp)) > 0) {
        send(fd, sendbuffer, byte, 0);
    }

    fclose(fp);
    return EXIT_SUCCESS;
}

int recvfile(const char* filename, const int fd)
{
    char buf[RECV_BUFSIZE + 1];
    int byte;
    int bytes_received = 0;

    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("Error opening file...\n");
    }

    while ((byte = recv(fd, buf, RECV_BUFSIZE, 0)) > 0) {
        bytes_received += byte;
        fwrite(buf, 1, byte, fp);
    }
    printf("Received bytes: %d\n", bytes_received);

    if (byte < 0) {
        perror("Error receiving file...\n");
    }

    fclose(fp);
    return EXIT_SUCCESS;
}
