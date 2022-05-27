#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

/*
多个线程以读或写方式访问共享数据
*/

int counter;
pthread_rwlock_t rwlock; //定义读写锁

void* th_write(void *arg){  //写线程

    int t;
    int i = (int)arg;

    while(1){
        pthread_rwlock_wrlock(&rwlock); // 以写模式加锁，写独占
        t = counter;
        usleep(1000);
        printf("=======write %d: %lu: counter = %d ++counter=%d\n", i, pthread_self(), t, ++counter);
        pthread_rwlock_unlock(&rwlock);
        usleep(10000);
    }
    return NULL;
}

void* th_read(void *arg){ //读线程
    int i = (int)arg;

    while(1){
        pthread_rwlock_rdlock(&rwlock);
        printf("----------------read %d: %lu: %d\n", i, pthread_self(), counter);
        pthread_rwlock_unlock(&rwlock);
        usleep(2000);
    }
    return NULL;
}


int main(){ //主线程

    int i;
    pthread_t tid[8];

    int ret = pthread_rwlock_init(&rwlock, NULL); //初始化读写锁
    if(ret != 0){
        perror("mutex init error");
        exit(1);
    }

    for(i=0; i<3; i++){ //3个写线程
        pthread_create(&tid[i],  NULL, th_write, (void*)i );
    }

    for(i=0; i<5; i++){ //5个读线程
        pthread_create(&tid[i+3], NULL, th_read, (void*)i);
    }


    for(i=0; i<8; i++)
        pthread_join(tid[i], NULL);

    pthread_rwlock_destroy(&rwlock);

    return 0;

}