/*
    #include <sys/types.h>
    #include <unistd.h>

    pid_t fork(void);
    功能：创建子进程
    返回值：
        父进程和子进程各自返回一个fork()执行结果
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]){

    printf("before fork -1\n");
    printf("before fork -2\n");
    printf("before fork -3\n");

    pid_t pid = fork(); // 成功返回pid
    //  On success, the PID of the child process is returned in the parent, and 0 is returned in  the  child
    if( pid == -1){
        perror("fork error");
        exit(1);
    }else if(pid == 0){
        printf("----child is created, pid = %d, parent-pid = %d\n", getpid(), getppid());
    }else if(pid > 0){
        printf("----parent process: my child is %d, my pid: %d, my parent pid :%d\n", pid, getpid(), getppid());
    }

    printf("-----end of file\n");

    return 0;

}