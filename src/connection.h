#ifndef CONNECTION_H
#define CONNECTION_H

#include  "../lib/http_parser.h"
#include  "util.h"
#include  "timer.h"

#include <netinet/in.h>
#include <sys/epoll.h>
#include <time.h>
#include <sys/stat.h>

//#define     BUFFSIZE    10*1024
#define    MAXCONNECTION    (10240)

struct HttpRequest;
struct HttpResponse;
struct Connection;
typedef struct HttpRequest http_request_t;
typedef struct HttpResponse http_response_t;
typedef struct Connection connection_t;

struct HttpRequest
{
    connection_t *req_connection;        // 所归属的Connetion结构体
    char inbuf[BUFFSIZE];
    //char outbuf[BUFFSIZE];
    ssize_t allrecved;
    int fd;
    int epoll_fd;
    int req_status;
    int method;
    char url[256];

    http_parser             parser;
    http_parser_settings settings;

};

struct HttpResponse
{
    int             fd;
    int             keep_alive;
    timer_t        mtime;
    int             modified;
    int             status;
};

struct Connection
{
    http_request_t           con_request;         // 对应的http请求
    int                          fd;                    // 连接对应的连接描述符（connfd）
    int                          epoll_fd;
    int                          keep_alive;
    struct epoll_event      event;                // epoll 事件配置
    struct sockaddr_in      saddr;                // 连接描述符对应socket
    time_t                      active_time;       // 上一次活跃时间
    int                          heap_idx;           // 当前连接在小根堆数组中的序号
};



void   RequestInit(connection_t *con);
int    RequestRecv(connection_t *con);
int    RequestParsed(connection_t *con);
int    RequestHandle(connection_t *con);

int    ResponseHandle(connection_t *con);

// http_parser_execute 执行过程中的7个回调函数
int OnMessageBeginCallback(http_parser *parser);
int OnUrlCallback(http_parser *parser, const char *at, size_t length);
int OnHeaderFieldCallback(http_parser *parser, const char *at, size_t length);
int OnHeaderValueCallback(http_parser *parser, const char *at, size_t length);
int OnHeadersCompleteCallback(http_parser *parser);
int OnBodyCallback(http_parser *parser);
int OnMessageCompleteCallback(http_parser *parser);

void connection_set_nodelay(connection_t *con);
// 定义为static函数，在该源文件可见，仅供connection_close()调用
void connection_free(connection_t *con);
int connection_close(connection_t *con);

connection_t* connection_accept(int conn_fd, int epoll_fd, struct sockaddr_in *paddr);

#endif // CONNECTION_H
