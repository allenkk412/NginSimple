#include "http.h"

void InitParser(connection_t *con)
{
    http_parser_settings settings;
    http_parser parser;

    bzero(&settings, sizeof(settings));

    settings.on_message_begin = OnMessageBeginCallback;
    settings.on_url = OnUrlCallback;
    settings.on_header_field = OnHeaderFieldCallback;
    settings.on_header_value = OnHeaderValueCallback;
    settings.on_headers_complete = OnHeadersCompleteCallback;
    settings.on_body = OnbodyCallback;
    settings.on_message_complete = OnMessageCompleteCallback;

    http_parser_init(&parser, HTTP_REQUEST);

    //http_parser_execute(&parser, &settings, const char *, size_t );

}


int OnMessageBeginCallback(http_parser *parser)
{
    return 0;
}

int OnUrlCallback(http_parser *parser, const char *at, size_t length)
{
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
    return 0;
}

int OnbodyCallback(http_parser *parser)
{
    return 0;
}

int OnMessageCompleteCallback(http_parser *parser);
{
    return 0;
}
