#include "connection.h"
#include "ns_epoll.h"
#include "socketfunc.h"
#include "http.h"


#include <netinet/in.h>       // struct sockaddr_in
#include <netinet/tcp.h>      // TCP_NODELAY
#include <sys/socket.h>       // socket,bind,listen,,,
#include <sys/epoll.h>        // epoll functions
#include <string.h>           // bzero()
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>


void RequestInit(connection_t *con)
{
    http_request_t *req = (http_request_t* )(&(con->con_request));
    req->req_connection = con;

    memset(req->inbuf, 0, BUFFSIZE);
    memset(req->url, 0, 256);

    req->allrecved = 0;
    req->fd = con->fd;
    req->epoll_fd = con->epoll_fd;
    req->req_status = 0;
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


int RequestRecv(connection_t *con)
{
    // 接收报文
    /**
     *  函数返回0  : client close or read EOF
     *  函数返回1  : 无数据可读
     *  函数返回-1 : 读数据出错
     */
    ssize_t    nrecved = 0;

    // nrecved > 0， 每次接收到的数据，添加进连接inbuf末尾，末尾偏移为con_request.allrecved,allrecved记录当前连接接收数据大小
    while((nrecved = recv(con->fd, con->con_request.inbuf + con->con_request.allrecved, sizeof(con->con_request.inbuf), 0)) > 0)
    {
        con->con_request.allrecved += nrecved;
    }

    // nrecved == 0, recv() retrun 0 on EOF
    // or "When a TCP connection is closed on one side read() on the other side returns 0 byte."
    if(nrecved == 0)
        return 0;

    if(nrecved < 0)
    {
        if(errno != EAGAIN)
        {
            printf("ERROR: recv() error");
            fflush(stdout);
            return -1;
        }else if (errno == EAGAIN){
            return 1;                    //非阻塞IO，无数据可读
        }
    }

    return 1;
/* old */
//    char buf[BUFFSIZE];
//    int len;
//
//    while(1)
//    {
//        len = recv(con->fd, buf, sizeof(buf), 0);
//
//        if(len == 0)                /* client close or read EOF */
//            return 0;
//
//        if(len == -1)
//        {
//            if(errno != EAGAIN)
//            {
//                printf("ERROR: recv error.");
//                fflush(stdout);
//                return -1;          /* recv error */
//            }else if(errno == EAGAIN)
//                return 1;           /* recv again*/
//        }
//
//        /* if len > 0*/
//        strncat( con->con_request.inbuf, buf, len);
//    }
//    return 1;
}

int RequestParsed(connection_t *con)
{
    int nparsed = http_parser_execute(&con->con_request.parser, &con->con_request.settings, con->con_request.inbuf, strlen(con->con_request.inbuf));
    if(con->con_request.parser.http_errno != HPE_OK)
    {
        printf("ERROR: http_parser_execute(), http_errno != HPE_OK");
        fflush(stdout);
        return -1;
    }

    return nparsed;
}

int RequestHandle(connection_t *con)
{
    /*
    * RequestRecv()
    * RequestParsed()
    *   函数返回1，完成请求的解析，在事件循环中完成接下来的处理
    *   函数返回0，解析未完成或连接关闭，退出当前事件处理
    */
    printf("enter HandleRequest()\n");
    int status = 0;

    // 接收报文
    status = RequestRecv(con);
    if(status == -1 || status == 0)  // error or client close
        goto err;

    /*
     *解析报文，解析状态存储在con->con_request.req_status中
    * req_status = 1,解析完成
    * req_status = 0,未解析完成
    * req_status在http_parser_execute()中的回调函数中设置
     */
    RequestParsed(con);
    if(con->con_request.req_status != 1)
        return 0; // http解析未完成，从RequestHandle函数返回，等下一次读事件触发

    return 1;   // 返回1，表示HTTP请求已解析完毕，进入事件处理循环完成接下来的工作

    err:
    //close:
    // 发生错误或客户端正常关闭
    // 关闭相应连接描述符号、删除epoll兴趣事件、释放Connection数据结构
    connection_close(con);
    return 0;    // 连接关闭，返回0，然后退出事件处理循环
}

int ResponseHandle(connection_t *con)
{
    struct stat sbuf;
    char filename[256];
    strcpy(filename, con->con_request.url);

    //strncpy(filename, con->con_request.url, strlen(con->con_request.url));
    // 默认请求index.html
    if(filename[strlen(filename) - 1] == '/'){
        strcat(filename, "index.html");
    }


    if(error_process(&sbuf, filename, con->fd))
        return -1;

    serve_static(con->fd, filename, sbuf.st_size, con);

    return 0;
}

int OnMessageBeginCallback(http_parser *parser)
{
    connection_t *c = (connection_t *)parser->data;
    c->con_request.req_status = 0;
    c->con_request.allrecved = 0;
    memset(c->con_request.url, 0 , sizeof(c->con_request.url));
    memset(c->con_request.inbuf, 0, sizeof(c->con_request.inbuf));

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

    c->con_request.req_status = 1;
    return 0;
}


void connection_set_nodelay(connection_t *con)
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
    setsockopt(con->fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
}

int connection_close(connection_t *con)
{
    if (con == NULL)
    return 0;

    ns_epoll_del(con->epoll_fd, con->fd, con);
    close(con->fd);

    // connection_unregister(c);

    // 释放c所指向的连接的Connection结构体空间，包括连接所对应的HttpRequest结构体空间
    free(con);

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
    RequestInit(c);

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
