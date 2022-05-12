#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

/** 
* 使用管道实现兄弟进程间通信。 兄： ls 弟： wc -l 父：等待回收子进
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
    int i;
    for(i=0; i<2; i++){
        pid = fork();

        if(pid == -1){
        perror("fork error");
        exit(1);
        }

        if(pid == 0) // 子进程
            break;
    }

    if(i == 2){ //父进程
        close(fd[0]);
        close(fd[1]);
        wait(NULL);
        wait(NULL);
    }else if(i == 0){ // 兄

        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO); //标准输出-> 管道
        execlp("ls", "ls", NULL);
        perror("execlp error");

    }else if(i == 1){ // 弟

        close(fd[1]); //关闭写端
        dup2(fd[0], STDIN_FILENO);
        execlp("wc", "wc", "-l", NULL); 
        perror("execlp error");

    }
   

    return 0;

}