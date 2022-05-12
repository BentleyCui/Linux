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

    p = (int*)mmap(NULL, 40, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0); 
    if(p == MAP_FAILED){
        perror("mmap error");
        exit(1);
    }


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

    int ret = munmap(p, 40); // 释放映射区
    if(ret == -1){
        perror("munmap error");
        exit(1);
    }
    return 0;

}