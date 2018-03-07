#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>  //inet_ntop()
#include <unistd.h>     //close()

#include "socketfunc.h"
//#include "error.h"
//#include "util.h"
//#include "connection.h"
#include "ns_epoll.h"

extern struct epoll_event* evlist;


int main()
{
    printf("hello world.\n");
    //int port = 2333;
    // 创建listen socket
    int listenfd = socket_bind_listen( 5000);
    set_fd_nonblocking(listenfd);

    struct sockaddr_in cliaddr;

    char rbuf[1024];
    char wbuf[1024];

    int epfd = ns_epoll_create(0);
    int fd, connfd, nfds, i, n, nread, nwrite;
    socklen_t addrlen;

    struct epoll_event ev;
    ev.data.fd = listenfd;
    ev.events = EPOLLIN;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1 )
        return -1;

   printf("epoll add listenfd.\n");
   for(;;)
    {
        printf("11\n");
        addrlen = sizeof(cliaddr);

        nfds =epoll_wait(epfd, evlist, MAXEVENTS, -1);
        printf("22\n");

        for( i = 0;i < nfds; i++)
        {
            fd = evlist[i].data.fd;
            if( fd == listenfd )
            {
                // 新连接事件: accept 连接，并将返回新的连接连接描述符添加到epfd的兴趣列表中
                while((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &addrlen) ) > 0)
                {
                    printf("new connection.");
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
                    printf("read.");
                    n = 0;
                    while((nread = read(fd, rbuf + n, sizeof(rbuf) - 1) ) > 0)
                        n += nread;

                    if( nread == -1 && errno != EAGAIN)
                        perror("read error");

                    ev.data.fd = fd;
                    ev.events = evlist[i].events | EPOLLOUT;
                    if( epoll_ctl( epfd, EPOLL_CTL_MOD, fd, &ev) == -1)
                        perror("epoll_ctl: mod");
                }

                if( evlist[i].events & EPOLLOUT )
                {
                // 处理写事件
                // do_write();
                    sprintf(wbuf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\nHellp World", 11);
                    nwrite = 0;
                    int data_size = strlen(wbuf);
                    n = data_size;
                    while(n > 0)
                    {
                        nwrite = write(fd, wbuf + data_size - n, n);
                        if(nwrite < n)
                        {
                            if(nwrite == -1 && errno != EAGAIN)
                                perror("write error");
                            break;
                        }

                        n -= nwrite;
                    }

                }

            }


        }




    }
   return 0;
}

