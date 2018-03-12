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

void InitRequest(connection_t *con)
{
    http_request_t *req = &(con->con_request);
    req->req_connection = con;

    memset(req->inbuf, 0, BUFFSIZE);
    memset(req->url, 0, 256);

    req->fd = con->fd;
    req->epoll_fd = con->epoll_fd;
    req->status = 0;
    req->method = 0;

    bzero(&(req->settings), sizeof(req->settings) );
    req->settings.on_message_begin = OnMessageBeginCallback;
    req->settings.on_url = OnUrlCallback;
    req->settings.on_header_field = OnHeaderFieldCallback;
    req->settings.on_header_value = OnHeaderValueCallback;
    req->settings.on_headers_complete = OnHeadersCompleteCallback;
    req->settings.on_body = OnbodyCallback;
    req->settings.on_message_complete = OnMessageCompleteCallback;

    http_parser_init(&(req->parser), HTTP_REQUEST);

    // 在回调函数中使用
    req->parser.data = con;

}

int HandleRequest(connection_t *con)
{
    int req_status = 0;

    return req_status;
}

int OnMessageBeginCallback(http_parser *parser)
{
    return 0;
}

int OnUrlCallback(http_parser *parser, const char *at, size_t length)
{
    connection_t *c = (connection_t*)parser->data;
    strncpy(c->con_request.url, at, length);

    printf("%s", c->con_request.url);
    fflush(stdout);

    return 0;
}

int OnHeaderFieldCallback(http_parser *parser, const char *at, size_t length)
{
    return 0;
}

int OnHeaderValueCallback(http_parser *parser, const char *at, size_t length)
{
    return 0;
}

int OnHeadersCompleteCallback(http_parser *parser)
{
    connection_t *c = (connection_t*)parser->data;
    c->con_request.method = parser->method;
    return 0;
}

int OnbodyCallback(http_parser *parser)
{
    return 0;
}

int OnMessageCompleteCallback(http_parser *parser)
{
    connection_t *c = (connection_t*)parser->data;

	if( http_should_keep_alive (parser) == 0 )
    {
        c->keep_alive = 0;
    }else{
        c->keep_alive = 1;
    }
    return 0;
}


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

    // ns_epoll_add()将指向连接的指针c作为用户定义数据传入epoll.data.ptr
    if( ns_epoll_add(epoll_fd, conn_fd, c, EPOLLIN | EPOLLET) == -1 )
    {
        // 关闭连接套接字，释放Connection结构体空间
        connection_close(c);
        return NULL;
    }

    return c;
}
