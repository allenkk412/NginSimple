#ifndef HTTP_H
#define HTTP_H

#include "../lib/http_parser.h"
#include "connection.h"


struct HttpRequest
{
    connection_t *c;        // 所归属的Connetion结构体

};

typedef struct HttpRequest http_request_t;

struct HttpResponse
{

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
