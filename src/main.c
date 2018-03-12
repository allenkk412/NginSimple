#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>  //inet_ntop()
#include <unistd.h>     //close()

#include "socketfunc.h"
#include "error.h"
#include "util.h"
#include "connection.h"
#include "ns_epoll.h"
#include "../lib/http_parser.h"

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
    int connfd, nfds, i, n, nread, nwrite;
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

        nfds = ns_epoll_wait(epfd, evlist, MAXEVENTS, 20);


        for( i = 0;i < nfds; i++)
        {
            // 获取指向当前处理事件的指针curr_event
            struct epoll_event *curr_event = evlist + i;
            int fd =  *((int *)(curr_event->data.ptr));
            if( fd == listenfd ){
                // accept新的连接，创建和初始化连接对应的Connection和HttpRequest结构体
                server_accept(listenfd, epfd);
            }else {
                // 处理已连接上的新的事件

                connection_t *c = curr_event->data.ptr;
                //int status;
                assert( c != NULL);

                //if(connection_is_expired(c))
                //    continue;

                if( curr_event.events & EPOLLIN )
                {
                    // 处理读事件
                    // do_read();


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

