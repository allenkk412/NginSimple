#include <iostream>
#include <string>

#include <string.h>

#include "http_parser.h"



int HttpOnMessageBeginCallback(http_parser *parser)
{
    std::cerr << __FUNCTION__ << std::endl;
    return 0;
}

int HttpOnUrlCallback(http_parser *parser, const char *at, size_t length)
{
    std::cerr << __FUNCTION__ << std::endl;
    return 0;
}

int HttpOnHeaderFieldCallback(http_parser *parser, const char *at, size_t length)
{
    std::cerr << __FUNCTION__ << std::endl;
    return 0;
}

int HttpOnHeaderValueCallback(http_parser *parser, const char *at, size_t length)
{
    std::cerr << __FUNCTION__ << std::endl;
    return 0;
}

int HttpOnHeadersCompleteCallback(http_parser *parser)
{
    std::cerr << __FUNCTION__ << std::endl;
    return 0;
}

int HttpOnBodyCallback(http_parser *parser, const char *at, size_t length)
{
    std::cerr << __FUNCTION__ << std::endl;
    return 0;
}

int HttpOnMessageCompleteCallback(http_parser *parser)
{

    std::cerr << __FUNCTION__ << std::endl;
    return 0;
}


int main(int argc, char* const argv[])
{
    // std::string request0 = "GET /index.html HTTP/1.1\r\nHost:localhost\r\nConnection:Keep-Alive\r\nAccept-";
    // std::string request1 = "Encoding:gzip,deflate\r\nAccept-L";
    // std::string request2 ="anguage:zh-cn\r\nContent-Lenght:5\r\n\r\nhello";
    // std::string request1 = "GET /index.html HTTP/1.1\r\nHost:localhost\r\nContent-Lenght:5\r\n\r\nhello";
    std::string request0 = "GET /ind";
    
    size_t size0 = request0.size() ;
    std::cout << size0 << std::endl;
   // std::cout << request1.size() << std::endl;

    http_parser          parser;
    http_parser_settings settings;
    
    bzero(&settings, sizeof(settings));
    settings.on_message_begin = HttpOnMessageBeginCallback;
    settings.on_url                   = HttpOnUrlCallback;
    settings.on_header_field     = HttpOnHeaderFieldCallback;
    settings.on_header_value   = HttpOnHeaderValueCallback;
    settings.on_headers_complete = HttpOnHeadersCompleteCallback;
    settings.on_body                    = HttpOnBodyCallback;
    settings.on_message_complete = HttpOnMessageCompleteCallback;
    //settings.on_headers_complete = HttpOnHeadersCompleteCallback;

    http_parser_init(&parser, HTTP_REQUEST);
    std::cerr << http_parser_execute(&parser, &settings, request0.c_str(), request0.size()) << std::endl;

    request0.append("ex.html HTTP/1.1\r\nHost:localhost\r\nConnection:Keep-Alive\r\nAccept-Encoding:gzip\r\nAccept-Language:zh-cn\r\nContent-Lenght:5\r\n\r\nhello");
    size_t size1 = request0.size() ;
    std::cout << size1 << std::endl;
    //std::cout << size1 << std::endl;

    std::cerr << http_parser_execute(&parser, &settings, request0.c_str(), size1) << std::endl;
    std::cerr << http_parser_execute(&parser, &settings, request0.c_str(), 0) << std::endl;


    return 0;
}
