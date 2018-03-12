#ifndef CONNECTION_H
#define CONNECTION_H

#include "http.h"

#include <netinet/in.h>
#include <sys/epoll.h>

struct HttpRequest;

struct Connection
{
    int                     fd;                // 连接对应的连接描述符（connfd）
    int                     epoll_fd;
    int                     keep_alive;
    struct epoll_event event;            // epoll 事件配置
    struct sockaddr_in saddr;            // 连接描述符对应socket
    time_t active_time;            // 上一次活跃时间
    HttpRequest con_request;                   // 对应的http请求
};

typedef struct Connection connection_t;

void connection_set_nodelay(connection_t *c);
int connection_close(connection_t *c);
connection_t* connection_accept(int conn_fd, int epoll_fd, struct sockaddr_in *paddr);

#endif // CONNECTION_H
