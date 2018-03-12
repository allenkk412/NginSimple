#include "http.h"
#include "connection.h"
#include "ns_epoll.h"
#include "socketfunc.h"


#include <netinet/in.h>       // struct sockaddr_in
#include <netinet/tcp.h>      // TCP_NODELAY
#include <sys/socket.h>       // socket,bind,listen,,,
#include <sys/epoll.h>        // epoll functions
#include <string.h>           // bzero()
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>


void connection_set_nodelay(connection_t *c)
{
    /*
      TCP_NODELAYIf set, disable the Nagle algorithm. This means that segments
      are always sent as soon as possible, even if there is only a small amount of data.
      When not set, data is buffered until there is a sufficient amount to send out,
      thereby avoiding the frequent sending of small packets, which results in poor utilization of the network.
      This option is overridden by TCP_CORK; however, setting this option forces an
      explicit flush of pending output, even if TCP_CORK is currently set.
    */

    int enable = 1;
    setsockopt(c->fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
}

int connection_close(connection_t *c)
{
    if (c == NULL)
    return 0;

    ns_epoll_del(c->epoll_fd, c->fd, c);
    close(c->fd);

    // connection_unregister(c);

    // 释放c所指向的连接的Connection结构体空间，包括连接所对应的HttpRequest结构体空间
    free(c);

    return 0;
}

connection_t* connection_accept(int conn_fd, int epoll_fd, struct sockaddr_in *paddr)
{
    // 配置coinnection_t内存空间
    // connection_t *c = connection_init();
    connection_t *c = malloc(sizeof(connection_t));
    if (c == NULL)
    {
        printf("ERROR: connetion malloc error.\n");
        fflush(stdout);
        return NULL;
    }

    // 初始化Connection结构体
    c->fd = conn_fd;
    c->epoll_fd = epoll_fd;
    if (paddr)
        c->saddr = *paddr;
    c->active_time = time(NULL);

    // 初始化连接对应的请求 c->req HttpRequest结构体
    InitRequest(c);

    set_fd_nonblocking(c->fd);
    connection_set_nodelay(c);

//    if (connection_register(c) == ERROR)
//    {
//        connection_close(c);
//        return NULL;
//    }

    if( ns_epoll_add(epoll_fd, conn_fd, c, EPOLLIN | EPOLLET) == -1 )
    {
        // 关闭连接套接字，释放Connection结构体空间
        connection_close(c);
        return NULL;
    }

    return c;
}
