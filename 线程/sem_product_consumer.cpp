/*
借助信号量模拟生产者消费者问题
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM 5

int queue[NUM];                     // 全局数组实现环形队列
sem_t blank_number, product_number; //空格信号量， 产品信号量
 
void *consumer(void *arg){
    int i = 0;

    while(1){
        sem_wait(&product_number);  //消费者将产品数--，为0则阻塞等待
        printf("--Consume ---%d\n", queue[i]);
        queue[i] = 0;
        sem_post(&blank_number);

        i = (i+1) % NUM;
        sleep(rand() % 3);
    }

}

void *producer(void *arg){
    int i = 0;
    while(1){
        sem_wait(&blank_number);  // 生产者将空格数--，为0则阻塞等待
        queue[i] = rand() % 1000 + 1;  //生产一个产品
        printf("--Produce-------%d\n", queue[i]);
        sem_post(&product_number);   //产品数++
        i = (i+1) % NUM;            //环形队列
        sleep(rand() % 1);
    }
}

int main(){ //主线程

    pthread_t pid, cid;
    srand(time(NULL));

    sem_init(&blank_number, 0, NUM);   //指定信号量初值
    sem_init(&product_number, 0, 0);  //初始化信号量，线程共享

    pthread_create(&pid, NULL, producer, NULL);
    pthread_create(&cid, NULL, consumer, NULL);

    pthread_join(pid, NULL);
    pthread_join(cid, NULL);

    sem_destroy(&blank_number);
    sem_destroy(&product_number);
   
    return 0;
}