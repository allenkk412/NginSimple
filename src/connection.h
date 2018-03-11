#ifndef CONNECTION_H
#define CONNECTION_H

#inc
#include <sys/epoll.h>

struct Connection{
    int                     fd;                // 连接对应的连接描述符（connfd）
    struct epoll_event event;            // epoll 事件配置
    struct sockaddr_in saddr;            // 连接描述符对应socket
    //time_t active_time;            // 上一次活跃时间
    http_request_t req;                   // 对应的http请求

    //

};

typedef struct Connection connection_t;



#endif // CONNECTION_H
