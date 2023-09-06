#ifndef _SENDFILE_H_
#define _SENDFILE_H_

int sendfile(char* file, int sfd);

int recvfile(char* file, int rfd);

#endif
