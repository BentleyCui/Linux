#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
FIFO的读操作
操作FIFO 类似于操作 文件
*/

int main(int argc, char* argv[]){

    int fd, len;
    char buf[4096];

    if(argc < 2){
        printf("Enter like this: ./a.out fifoname\n");
        return -1;
    }

    fd = open(argv[1], O_RDONLY); // 打开fifo
    if(fd < 0){
        perror("open fifo error");
        exit(1);
    }

    while(1){
        len = read(fd, buf, sizeof(buf)); // 从FIFO读到buf
        write(STDOUT_FILENO, buf, len); // 标准输出，打印到终端
        sleep(1);
    }

    close(fd);

    return 0;
}