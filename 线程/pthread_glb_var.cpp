#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

int var = 100;

void* tfn(void *arg){
    var = 200;
    printf("thread\n");

    return NULL;
}

int main(){ //主线程

    printf("at first var = %d\n", var);

    pthread_t tid;
    int ret = pthread_create(&tid,  NULL, tfn, NULL);
    if(ret != 0){
        perror("pthread_create error");
    }

    sleep(2); //需要给子线程留出执行时间 

    printf("after pthread_create, var = %d\n", var);

    return 0;

}