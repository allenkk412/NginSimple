#include "ns_epoll.h"
#include <stdlib.h>
#include <stdio.h>
#include "connection.h"

struct epoll_event* evlist;      // epoll_wait() 内核返回的就绪event数组

int ns_epoll_create(int flags)
{
    int epoll_fd = epoll_create1(flags);
    if( epoll_fd == -1)
    {
        perror("epoll_cteate ");
        return -1;
    }
    evlist = (struct epoll_event*)malloc(sizeof(struct epoll_event) * MAXEVENTS);

    return epoll_fd;
}

int ns_epoll_add(int epoll_fd, int fd, connection_t *c, uint32_t events)
{
    int rc;
    struct epoll_event event;
    event.data.ptr = c;
    event.events = events;
    rc = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, c->fd, &event);

    if(rc == -1)
    {
        perror("epoll_ctl : add listenfd");
        return -1;
    }

    return rc;
}

int ns_epoll_del(int epoll_fd, int fd, connection_t *c, uint32_t events)
{
    int rc;
    struct epoll_event event;
    event.data.ptr = c;
    event.events = events;
    rc = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, c->fd, &event);

    if(rc == -1)
    {
        perror("epoll_ctl : mod");
        return -1;
    }

    return rc;

}


int ns_epoll_mod(int epoll_fd, int fd, connection_t *c, uint32_t events)
{
    int rc;
    struct epoll_event event;
    event.data.ptr = c;
    event.events =events;
    rc = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, c->fd, &event);

    if(rc == -1)
    {
        perror("epoll_ctl : mod");
        return -1;
    }

    return rc;
}

int ns_epoll_wait( int epoll_fd, struct epoll_event *evs, int max_events, int timeout)
{
    return epoll_wait(epoll_fd, evs, max_events, timeout);
}
