#ifndef _SENDFILE_H_
#define _SENDFILE_H_

int
sendfile(const char* filename, const int fd);
int
recvfile(const char* filename, const int fd);

#endif
