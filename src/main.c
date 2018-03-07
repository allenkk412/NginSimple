#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>  //inet_ntop()
#include <unistd.h>     //close()

#include "socketfunc.h"
#include "error.h"
#include "util.h"
#include "connection.h"
#include "epoll.h"




int main(int argc, char **argv)
{
    int port = 2333;
    // 创建listen socket
    int listenfd = socket_bind_listen( port );
    set_fd_nonblocking(listenfd);

    sockaddr_in cliaddr;

    int epfd = ns_epoll_create(0);
    int nfds, i,fd , connfd;
    socklen_t addrlen;

    struct epoll_event ev;
    ev.data.fd = listenfd;
    ev.events = EPOLLIN;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1 )
        return -1;


    for(;;)
    {
        addrlen = sizeof(cliaddr);
        nfds =epoll_wait(epfd, evlist, MAXEVENTS, -1);

        for( i = 0;i < nfds; i++)
        {
            fd = evlist[i].data.fd;
            if( fd == listenfd )
            {
                // 新连接事件: accept 连接，并将返回新的连接连接描述符添加到epfd的兴趣列表中
                while((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &addrlen) ) > 0)
                {
                    // 循环抱住accept调用: ET模式，多个就绪连接到达时候，只会通知一次，accept只处理一个连接
                    set_fd_nonblocking(connfd);
                    ev.data.fd = connfd;
                    ev.events = EPOLLIN | EPOLLET;
                }

            }else {
                if( evlist[i].events & EPOLLIN )
                {
                // 处理读事件
                // do_read();
                }

                if( evlist[i].events & EPOLLIN )
                {
                // 处理写事件
                // do_write();
                }

            }


        }




    }

    return 0;
}
