#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>

int main(){
    
    char *p = NULL;
    int fd;

    fd = open("test_mmap", O_RDWR | O_CREAT | O_TRUNC, 0644); //打开文件
    if(fd == -1){
        perror("open error");
        exit(1);
    }

    lseek(fd, 20, SEEK_END); //拓展文件大小
    write(fd, "\0", 1);
    int len = lseek(fd, 0, SEEK_END);

    p = (char*)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
    if(p == MAP_FAILED){
        perror("mmap error");
        exit(1);
    }

    // 使用指针p操作文件，对文件进程读写操作
    strcpy(p, "hello mmap");
    printf("------%s\n", p);

    int ret = munmap(p, len);
    if(ret == -1){
        perror("munmap error");
        exit(1);
    }
    return 0;

}