#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/** 
* ls | wc -l 命令
* 父进程实现ls, 子进程实现wc -l，通过管道实现数据通信
**/

int main(){

    int ret;
    int fd[2];
    ret = pipe(fd); //创建管道

    if(ret == -1){
        perror("pipe error");
        exit(1);
    }

    pid_t pid;
    pid = fork(); // 创建子进程

    if(pid > 0){    //  父进程:写端

        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO); //标准输出-> 管道
        execlp("ls", "ls", NULL);
        perror("execlp error");
        

    }else if(pid == 0){ //子进程:读端

        close(fd[1]); //关闭写端
        dup2(fd[0], STDIN_FILENO);
        execlp("wc", "wc", "-l", NULL); 
        perror("execlp error");

    }else if(pid == -1){
        perror("fork error");
        exit(1);
    }

    return 0;

}