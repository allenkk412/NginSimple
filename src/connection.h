#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/epoll.h>

struct connection{
    int                     fd;                // 连接对应的连接描述符（connfd）
    struct epoll_event event;            //
    struct sockaddr_in saddr;            //

    //

};

typedef struct connection connection_t;



#endif // CONNECTION_H
