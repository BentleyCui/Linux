#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

void *tfn(void *arg){ // 子线程

    printf("thread: pid = %d, tid = %lu\n", getpid(), pthread_self());
    return NULL;

}

int main(){ //主线程

    pthread_t tid;

    printf("main: pid = %d, tid = %lu\n", getpid(), pthread_self());

    int ret = pthread_create(&tid,  NULL, tfn, NULL);
    if(ret != 0){
        perror("pthread_create error");
    }

    sleep(2); //需要给子线程留出执行时间 

    return 0;

}