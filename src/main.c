#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>       //struct sockaddr_in
#include <sys/socket.h>       //socket,bind,listen,,,
#include <sys/epoll.h>        //epoll functions
#include <fcntl.h>            //fcntl

#include "error.h"
#include "util.h"

int set_fd_nonblocking( int fd )
{
    int flag = fcntl(fd, F_GETFL, 0) ;
    if( flag < 0 )
        return -1;
    if(fcntl(fd, F_SETFL, flag | O_NONBLOCK) < 0)
        return -1;

    return 0;
}


int main(int argc, char **argv)
{
    int port = 2333;
    int listenfd;
    struct sockaddr_in listenaddr,cliaddr;

    // 创建socket，返回监听描述符
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
        return -1;

    // 设置listenfd为非阻塞
    set_fd_nonblocking(listenfd);

    // 消除bind时的"Address already in use."错误
    int reuse = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
        return -1;

    // 设置服务器IP和Port，绑定协议地址和监听描述符
    bzero(&listenaddr, sizeof(listenaddr));
    listenaddr.sin_family          = AF_INET;
    listenaddr.sin_addr.s_addr   = htonl(INADDR_ANY);  //内核决定监听IP地址
    listenaddr.sin_port            = htons(port);

    if ( bind(listenfd, (struct sockaddr*)&listenaddr, sizeof(listenaddr)) < 0 )
	{
		return -1;
	}

    // 开始监听，最大等待队列长为LISTENQ(未完成连接队列 + 已完成连接队列）
    if (listen(listenfd, LISTENQ) == -1 )
	{
		return -1;
	}

    for(;;)
    {

    }

    return 0;
}
