#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>  //inet_ntop()
#include <unistd.h>     //close()

#include "socketfunc.h"
#include "error.h"
#include "util.h"




int main(int argc, char **argv)
{
    int port = 2333;
    int listenfd = socket_bind_listen( port );

    int connfd;
    socklen_t len;
    struct sockaddr_in cliaddr;
    char buff[BUFLEN];

    for(;;)
    {
        len = sizeof(cliaddr);
        if((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len)) == -1)
            return -1;

        printf("connection from %s, port %d \n", inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));

        if( close(connfd) == -1)
            return -1;

    }

    return 0;
}
