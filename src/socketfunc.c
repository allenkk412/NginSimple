#include "socketfunc.h"
#include "util.h"
#include "ns_epoll.h"
#include "connection.h"

#include <netinet/in.h>       //struct sockaddr_in
#include <sys/socket.h>       //socket,bind,listen,,,
#include <sys/epoll.h>        //epoll functions
#include <fcntl.h>            //fcntl()
#include <string.h>           //bzero()
#include <stdio.h>
#include <time.h>

int set_fd_nonblocking( int fd )
{
    int flag = fcntl(fd, F_GETFL, 0) ;
    if( flag < 0 )
    {
        printf("123");
        return -1;
    }

    if(fcntl(fd, F_SETFL, flag | O_NONBLOCK) < 0)
    {
        printf("123");
        return -1;
    }
    return 0;
}

int socket_bind_listen(int port)
{
    int listenfd;
    struct sockaddr_in listenaddr;

    // 创建socket，返回监听描述符
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
         printf("123");
         return -1;
    }

    //// 设置listenfd为非阻塞
    //set_fd_nonblocking(listenfd);

    // 消除bind时的"Address already in use."错误
    int reuse = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
    {
        printf("123");
        return -1;
    }

    // 设置服务器IP和Port，绑定协议地址和监听描述符
    bzero(&listenaddr, sizeof(listenaddr));
    listenaddr.sin_family          = AF_INET;
    listenaddr.sin_addr.s_addr   = htonl(INADDR_ANY);  //内核决定监听IP地址
    listenaddr.sin_port            = htons(port);

    if ( bind(listenfd, (struct sockaddr*)&listenaddr, sizeof(listenaddr)) == -1 )
    {
        printf("123");
        return -1;
    }

    // 开始监听，最大等待队列长为LISTENQ(未完成连接队列 + 已完成连接队列）
    if (listen(listenfd, LISTENQ) == -1 )
    {
        printf("123");
        return -1;
    }

    return listenfd;
}

int server_accept(int listen_fd, int epoll_fd)
{
    int conn_fd;
    // 设置为静态局部变量，减少内存分配和释放的操作次数
    // static struct sockaddr_in saddr;
    struct sockaddr_in saddr;
    socklen_t saddrlen = sizeof(struct sockaddr_in);

    // 新连接事件: accept 连接，并将返回新的连接连接描述符添加到epfd的兴趣列表中
    while((conn_fd = accept(listenfd, (struct sockaddr *) &saddr, &saddrlen) ) > 0)
    {
                // 循环抱住accept调用: ET模式，多个就绪连接到达时候，只会通知一次，accept只处理一个连接
        connection_accept(conn_fd, epoll_fd,&saddr);
        printf("new connection.\n");
        fflush(stdout);

    }

    return 0;

}
