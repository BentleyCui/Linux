#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int var = 100; // 全局变量

int main(){

    pid_t pid;
    pid = fork();

    if(pid == -1){
        perror("fork error");
        exit(1);
    }else if(pid > 0){
        var = 288;
        printf("parent, var = %d\n", var);
        printf("I'am parent pid = %d, getpid = %d\n", getpid(), getppid());
    }else if(pid == 0){
        var = 200;
        printf("child, var = %d\n", var);
        printf("I'am child pid = %d, getpid = %d\n", getpid(), getppid());
    }

    return 0;


}