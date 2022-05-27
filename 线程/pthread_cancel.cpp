#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

void *tfn(void *arg){ // 子线程

    while(1){
        printf("thread: pid = %d, tid = %lu\n", getpid(), pthread_self());
        sleep(1);
    }
    
    return NULL;
}

int main(){ //主线程

    pthread_t tid;

    int ret = pthread_create(&tid,  NULL, tfn, NULL); 
    if(ret != 0){
        perror("pthread_create error");
        exit(1);
    }

    printf("main: pid = %d, tid = %lu\n", getpid(), pthread_self());

    sleep(5);

    ret = pthread_cancel(tid);  // 杀死线程
    if(ret != 0){
        perror("pthread_cancel error");
        exit(1);
    } 

    while(1);
    
    pthread_exit((void*)0);

}