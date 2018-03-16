#include <stdio.h>
#include <string.h>

#include "http_parser.h"

int OnMessageBeginCallback(http_parser *parser)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

int OnUrlCallback(http_parser *parser, const char *at, size_t length)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

int OnHeaderFieldCallback(http_parser *parser, const char *at, size_t length)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

int OnHeaderValueCallback(http_parser *parser, const char *at, size_t length)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

int OnHeadersCompleteCallback(http_parser *parser)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

int OnBodyCallback(http_parser *parser)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

int OnMessageCompleteCallback(http_parser *parser)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

int main()
{
    char request[256] = "GET /index.html HTTP/1.1\r\nHost:test.py\r\nConnection:Keep-Alive\r\nContent-Length:5\r\n\r\nHello";

    http_parser          parser;
    http_parser_settings settings;
    
    bzero(&settings, sizeof(settings));
    settings.on_message_begin = OnMessageBeginCallback;
    settings.on_url                   = OnUrlCallback;
    settings.on_header_field     = OnHeaderFieldCallback;
    settings.on_header_value   = OnHeaderValueCallback;
    settings.on_headers_complete = OnHeadersCompleteCallback;
    settings.on_body                    = OnBodyCallback;
    settings.on_message_complete = OnMessageCompleteCallback;

    http_parser_init(&parser, HTTP_REQUEST);
    int nparsed = http_parser_execute(&parser, &settings, request, strlen(request)); 
     if(parser.http_errno != HPE_OK)
    {
        printf("ERROR: http_parser_execute(), http_errno != HPE_OK");
        fflush(stdout);
        return -1;
    }
    printf("%d, %d", strlen(request), nparsed);

    return 0;
}
