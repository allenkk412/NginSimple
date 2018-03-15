#ifndef RIO_H
#define RIO_H

#include <unistd.h>

// CSAPP 2E Page600

ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);

#endif
