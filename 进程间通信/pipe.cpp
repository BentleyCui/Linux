#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


int main(){
    int ret;
    int fd[2];
    char buf[1024];
    ret = pipe(fd); //创建管道

    if(ret == -1){
        perror("pipe error");
        exit(1);
    }

    pid_t pid;
    pid = fork(); // 创建子进程
    char *str = "hello pipe\n";

    if(pid > 0){    //  父进程：若让父进程写，子进程读

        close(fd[0]); //关闭读端
        sleep(3); //若sleep在这，会阻塞read
        write(fd[1], str, strlen(str));
        //sleep(2);
        close(fd[1]);

    }else if(pid == 0){ //子进程

        close(fd[1]); //关闭写端
        ret = read(fd[0], buf, sizeof(buf));
        write(STDOUT_FILENO, buf, ret);
        close(fd[0]);

    }

    return 0;

}