#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>


int main(int argc, char* argv[]){
    pid_t pid = fork();

    if(pid > 0){
        while(1){
            printf("parent, pid = %d\n", getpid());
            sleep(1);
        } 
        
    }else if(pid == 0){
        
        printf("child, pid = %d, ppid = %d\n", getpid(), getppid());
        sleep(4);
        kill(getppid(), SIGKILL); // 信号，无条件终止进程
    }

}