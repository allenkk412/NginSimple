#include "http.h"
#include "rio.h"
#include "util.h"


#include <time.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

int error_process(struct stat* sbufptr, char *filename, int fd)
{
        // 处理文件找不到错误
    if(stat(filename, sbufptr) < 0)
    {
        do_error(fd, filename, "404", "Not Found", "Can't find the file");
        return 1;
    }

    // 处理权限错误
    //S_IFREG :常规文件、S_IRUSR:文件所有者具有可读取权限
    if(!(S_ISREG((*sbufptr).st_mode)) || !(S_IRUSR & (*sbufptr).st_mode))
    {
        do_error(fd, filename, "403", "Forbidden", "Can't read the file");
        return 1;
    }

    return 0;

}

// 响应错误信息
void do_error(int fd, char *cause, char *err_num, char *short_msg, char *long_msg)
{
    // 响应头缓冲（512字节）和数据缓冲（10240字节）
    char header[512];
    char body[BUFFSIZE];

    // 用log_msg和cause字符串填充错误响应体
    sprintf(body, "<html><title>NginSimple Error<title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\n", body);
    sprintf(body, "%s%s : %s\n", body, err_num, short_msg);
    sprintf(body, "%s<p>%s : %s\n</p>", body, long_msg, cause);
    sprintf(body, "%s<hr><em>NginSimple web server</em>\n</body></html>", body);

    // 返回错误码，组织错误响应头
    sprintf(header, "HTTP/1.1 %s %s\r\n", err_num, short_msg);
    sprintf(header, "%sServer: NginSimple\r\n", header);
    sprintf(header, "%sContent-type: text/html\r\n", header);
    sprintf(header, "%sConnection: close\r\n", header);
    sprintf(header, "%sContent-length: %d\r\n\r\n", header, (int)strlen(body));

    // Add 404 Page

    // 发送错误信息
    rio_writen(fd, header, strlen(header));
    rio_writen(fd, body, strlen(body));
    return;
}

// 处理静态文件请求
void serve_static(int fd, char *filename, size_t filesize, connection_t *con)
{
    // 响应头缓冲（512字节）
    char header[512];


    // 返回响应报文头，包含HTTP版本号状态码及状态码对应的短描述
    sprintf(header, "HTTP/1.1 %d %s\r\n", 200 , "OK");

    // 返回响应头
    // Connection，Keep-Alive，Content-type，Content-length，Last-Modified
    if(con->keep_alive){
        // 返回默认的持续连接模式及超时时间（默认500ms）
        sprintf(header, "%sConnection: keep-alive\r\n", header);
    }
    // 通过Content-length返回文件大小
    sprintf(header, "%sContent-length: %zu\r\n", header, filesize);

    sprintf(header, "%sServer : NginSimple\r\n", header);

    // 空行（must）
    sprintf(header, "%s\r\n", header);

    // 发送报文头部并校验完整性
    size_t send_len = (size_t)rio_writen(fd, header, strlen(header));
    if(send_len != strlen(header)){
        perror("Send header failed");
        return;
    }

    // 打开并发送文件
    int src_fd = open(filename, O_RDONLY, 0);
    // mmap()系统调用在调用进程的虚拟地址空间中创建一个新的映射
    char *src_addr = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, src_fd, 0);
    close(src_fd);

    // 发送文件并校验完整性
    send_len = rio_writen(fd, src_addr, filesize);
    if(send_len != filesize){
        perror("Send file failed");
        return;
    }
    munmap(src_addr, filesize);
}
