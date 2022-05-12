#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char* argv[]){

    pid_t pid = fork(); // 成功返回pid
    //  On success, the PID of the child process is returned in the parent, and 0 is returned in  the  child
    
    int status;
    pid_t wpid;

    if( pid == -1){
        perror("fork error");
        exit(1);
    }else if(pid == 0){ //子进程
        printf("----child is created, my pid = %d\n", getpid());
        printf("sleep 10s\n");
        sleep(10);
        printf("-------------child die--------\n");
    }else if(pid > 0){ // 父进程
        wpid = wait(&status);  // 父进程会阻塞等待子进程退出
        if(wpid == -1){
            perror("wait error");
            exit(1);
        }

        if( WIFEXITED(status) ){ //为真，说明进程正常终止
            
        }

        printf("-------------parent wait finish: %d\n",wpid);
    }

    printf("-----end of file\n");

    return 0;

}