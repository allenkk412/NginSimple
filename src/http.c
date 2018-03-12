#include "http.h"

#include <string.h>

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

int OnMessageCompleteCallback(http_parser *parser);
{
    connection_t *c = (connection_t*)parser->data;

	if( http_should_keep_alive (parser) == 0 )
    {
        c->keep_alive = false;
    }else{
        c->keep_alive = true;
    }
    return 0;
}
