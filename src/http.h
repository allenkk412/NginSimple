#ifndef HTTP_H
#define HTTP_H

#include "connection.h"

#include <sys/stat.h>
#include <sys/types.h>


int error_process(struct stat* sbufptr, char *filename, int fd);
void do_error(int fd, char *cause, char *err_num, char *short_msg, char *long_msg);
void serve_static(int fd, char *filename, size_t filesize, connection_t *con);
#endif // HTTP_H
