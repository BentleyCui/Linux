#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

/*
主子线程共同访问公共资源stdout，需要加锁
*/

pthread_mutex_t mutex; //定义一把互斥锁

void *tfn(void *arg){ // 子线程

    srand(time(NULL));

    while(1){
        pthread_mutex_lock(&mutex); //加锁

        printf("hello ");
        sleep(rand() % 3);  //模拟长时间操作共享资源，导致cpu易主
        printf("world\n");

        pthread_mutex_unlock(&mutex); //解锁

        sleep(rand() % 3);
    }
    return NULL;

}

int main(){ //主线程

    pthread_t tid;
    srand(time(NULL));

    int ret = pthread_mutex_init(&mutex, NULL); // 初始化互斥锁
    if(ret != 0){
        perror("mutex init error");
        exit(1);
    }

    ret = pthread_create(&tid,  NULL, tfn, NULL);
    if(ret != 0){
        perror("pthread_create error");
    }

    while(1){

        pthread_mutex_lock(&mutex); //加锁
        printf("HELLO ");
        sleep(rand() % 3); 
        printf("WORLD\n");

        pthread_mutex_unlock(&mutex); //解锁
        sleep(rand() % 3); 
    }
    
    pthread_join(tid, NULL);
    pthread_mutex_destroy(&mutex);

    return 0;

}