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

    int epfd = ns_epoll_create(0);
    int nfds, i;

    struct epoll_event ev;
    ev.data.fd = listenfd;
    ev.events = EPOLLIN;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1 )
        return -1;

   printf("epoll add listenfd.\n");
   for(;;)
    {
        //addrlen = sizeof(cliaddr);

        nfds = ns_epoll_wait(epfd, evlist, MAXEVENTS, 20);

        for( i = 0;i < nfds; i++)
        {
            printf("11\n");
            // 获取指向当前处理事件的指针curr_event
            struct epoll_event *curr_event = evlist + i;
            printf("22\n");
            //int fd =  *((int *)(curr_event->data.ptr));
            int fd = curr_event->data.fd;
            printf("33\n");
            if( fd == listenfd ){
                printf("44\n");
                // accept新的连接，创建和初始化连接对应的Connection和HttpRequest结构体
                server_accept(listenfd, epfd);
            }else {
                // 处理已连接上的新的事件
                connection_t *c = (connection_t *)(curr_event->data.ptr);

                // 排除错误事件
                if (((*curr_event).events & EPOLLERR) || ((*curr_event).events & EPOLLHUP)
                    || (!((*curr_event).events & EPOLLIN)))
                {
                    connection_close(c);
                    continue;
                }

                //if(connection_is_expired(c))
                //    continue;

                if( curr_event->events & EPOLLIN )
                {
                    // 处理读事件
                    // do_read();
                    //status = HandleRequest(c);
                    if(RequestHandle(c) == 0)
                        continue;       // 返回0，结束当前事件处理

                    ResponseHandle(c);

                    if(!c->keep_alive)
                        connection_close(c);

//                    if(c->keep_alive)
//                        ns_epoll_mod(epfd, c->fd, c, (EPOLLIN | EPOLLET));
//                    else
//                        //ns_epoll_del(epfd, c->fd, c);
//                        connection_close(c);

                }


            }


        }

    }
   return 0;
}

