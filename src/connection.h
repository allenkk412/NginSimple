#ifndef CONNECTION_H
#define CONNECTION_H

#include "../lib/http_parser.h"

#include <netinet/in.h>
#include <sys/epoll.h>
#include <time.h>

#define     BUFFSIZE    10*1024

struct HttpRequest
{
    struct Connection *req_connection;        // 所归属的Connetion结构体
    char inbuf[BUFFSIZE];
    //char outbuf[BUFFSIZE];

    int fd;
    int epoll_fd;
    int status;
    int method;
    char url[256];

    http_parser             parser;
    http_parser_settings settings;

};

typedef struct HttpRequest http_request_t;

struct HttpResponse
{
    int fd;
    int keep_alive;
    timer_t mtime;
    int modified;
    int status;
};

typedef struct HttpResponse http_response_t;

struct Connection
{
    struct HttpRequest  con_request;                   // 对应的http请求
    int                     fd;                // 连接对应的连接描述符（connfd）
    int                     epoll_fd;
    int                     keep_alive;
    struct epoll_event event;            // epoll 事件配置
    struct sockaddr_in saddr;            // 连接描述符对应socket
    time_t active_time;            // 上一次活跃时间
};

typedef struct Connection connection_t;

void InitRequest(connection_t *con);
int HandleRequest(connection_t *con);

// http_parser_execute 执行过程中的7个回调函数
int OnMessageBeginCallback(http_parser *parser);
int OnUrlCallback(http_parser *parser, const char *at, size_t length);
int OnHeaderFieldCallback(http_parser *parser, const char *at, size_t length);
int OnHeaderValueCallback(http_parser *parser, const char *at, size_t length);
int OnHeadersCompleteCallback(http_parser *parser);
int OnbodyCallback(http_parser *parser);
int OnMessageCompleteCallback(http_parser *parser);

void connection_set_nodelay(connection_t *c);
int connection_close(connection_t *c);
connection_t* connection_accept(int conn_fd, int epoll_fd, struct sockaddr_in *paddr);

#endif // CONNECTION_H
