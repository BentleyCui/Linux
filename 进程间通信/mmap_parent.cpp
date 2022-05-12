#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>


int var = 100;

int main(){
    
    int *p = NULL;
    int fd;

    fd = open("test_mmap", O_RDWR | O_CREAT | O_TRUNC, 0644); //打开文件
    if(fd == -1){
        perror("open error");
        exit(1);
    }

    // lseek(fd, 20, SEEK_END); //拓展文件大小     两者等价于 ftruncate();
    // write(fd, "\0", 1);
    ftruncate(fd, 4);
    int len = lseek(fd, 0, SEEK_END);

    p = (int*)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
    if(p == MAP_FAILED){
        perror("mmap error");
        exit(1);
    }
    close(fd);

    // 前面为父进程创建文件映射区，指定大小

    pid_t pid;
    pid = fork();  //创建子进程

    if(pid == 0){
        *p = 2000;  // 写共享内存
        var = 1000; // 修改全局变量，读时共享，写时复制
        printf("child, *p = %d, var = %d\n", *p, var);
    }else{
        sleep(1);
        printf("parent, *p = %d, var = %d\n", *p, var);  // 读，父进程复制一份全局变量
        wait(NULL); // 回收子进程
    }

    int ret = munmap(p, len); // 释放映射区
    if(ret == -1){
        perror("munmap error");
        exit(1);
    }
    return 0;

}