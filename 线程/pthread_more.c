#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

void *tfn(void *arg){ // 子线程
    int i = (int)arg;
    sleep(i);
    printf("--I'm %dth thread: pid = %d, tid = %lu\n", i+1, getpid(), pthread_self());
    return NULL;

}

int main(){ //主线程

    pthread_t tid;
    int i;
    int ret;

    for(i=0; i<5; i++){
        ret = pthread_create(&tid, NULL, tfn, (void*)i); //传参采用值传递，借助强制类型转换
        if(ret != 0){
            perror("pthread_create rerror");
        }
    }

    sleep(i);
    printf("main: pid = %d, tid = %lu\n", getpid(), pthread_self());
    return 0;

}