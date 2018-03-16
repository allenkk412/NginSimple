#include <iostream>
#include <string>

#include <string.h>

#include "http_parser.h"

/* If http_should_keep_alive() in the on_headers_complete or
 * on_message_complete callback returns 0, then this should be
 * the last message on the connection.
 * If you are the server, respond with the "Connection: close" header.
 * If you are the client, close the connection.
 */

static bool ifkeepalive = 0;

int HttpOnMessageBeginCallback(http_parser *parser)
{
    std::cerr << __FUNCTION__ << std::endl;
    return 0;
}

int HttpOnMessageEndCallback(http_parser *parser)
{
    int i = 0;
	if( (i = http_should_keep_alive (parser) ) == 0 )
    {
        ifkeepalive = false;
    }else
    {
        ifkeepalive = true;
    }

    std::cerr << __FUNCTION__ << std::endl;
    return 0;
}

int main(int argc, char* const argv[])
{
    std::string request = "GET /index.html HTTP/1.1\r\nHost:localhost\r\nConnection:Keep-Alive\r\nContent-Lenght:5\r\n\r\nhello";

    std::string request1 = "GET /index.html HTTP/1.1\r\nHost:localhost\r\nContent-Lenght:5\r\n\r\nhello";

    std::cout << request.size() << std::endl;
    std::cout << request1.size() << std::endl;

    http_parser          parser;
    http_parser_settings settings;
    
    bzero(&settings, sizeof(settings));
    settings.on_message_begin = HttpOnMessageBeginCallback;
    settings.on_message_complete = HttpOnMessageEndCallback;
    //settings.on_headers_complete = HttpOnHeadersCompleteCallback;

    http_parser_init(&parser, HTTP_REQUEST);
    std::cerr << http_parser_execute(&parser, &settings, request.c_str(), 90) << std::endl;

    std::cout <<"keep-alive: "  <<  ifkeepalive << std::endl;

    return 0;
}
