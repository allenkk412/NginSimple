#ifndef HTTP_H
#define HTTP_H

#include "../lib/http_parser.h"
#include "connection.h"

#include <time.h>

#define     BUFFSIZE    10*1024

struct HttpRequest
{
    connection_t *c;        // 所归属的Connetion结构体
    char inbuf[BUFFSIZE];
    char outbuf[BUFFSIZE];

    int fd;
    int epoll_fd;
    int status;
    int method;

    http_parser          parser;
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


void InitParser(connection_t *con);

// http_parser_execute 执行过程中的7个回调函数
int OnMessageBeginCallback(http_parser *parser);
int OnUrlCallback(http_parser *parser, const char *at, size_t length);
int OnHeaderFieldCallback(http_parser *parser, const char *at, size_t length);
int OnHeaderValueCallback(http_parser *parser, const char *at, size_t length);
int OnHeadersCompleteCallback(http_parser *parser);
int OnbodyCallback(http_parser *parser);
int OnMessageCompleteCallback(http_parser *parser);

#endif // HTTP_H
