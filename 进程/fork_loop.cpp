#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// 循环创建n个子进程
int main(){
    int i;
    pid_t pid;

    for(i = 0; i<5; i++){
        pid = fork();
        if(pid == -1){
            // 创建子进程失败
            perror("create fork error");
            exit(1);
        }else if(pid == 0){
            // 子进程，停止fork()
            break;
        }
    }

    if(i == 5){
        sleep(5);
        printf("parent \n");
    }
    else{
        sleep(i);
        printf("child:%d \n", i);
    } 
       

    return 0;

}