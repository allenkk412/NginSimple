#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
    char filename[256] = "/index.html";
    struct stat st;
     if(stat(filename, &st) < 0)
    {
        printf("can't find file.\n");
        return 1;
    }

    // 处理权限错误
    //S_IFREG :常规文件、S_IRUSR:文件所有者具有可读取权限
    if(!(S_ISREG(st.st_mode)) || !(S_IRUSR & st.st_mode))
    {
        printf("can't read file.\n");
        return 1;
    }

    // // 打开并发送文件
    // int src_fd = open(filename, O_RDONLY, 0);
    // // mmap()系统调用在调用进程的虚拟地址空间中创建一个新的映射
    // char *src_addr = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, src_fd, 0);
    // close(src_fd);

    // // 发送文件并校验完整性
    // send_len = rio_writen(fd, src_addr, filesize);
    // if(send_len != filesize){
    //     perror("Send file failed");
    //     return;
    // }
    // munmap(src_addr, filesize);

    return 0;
}