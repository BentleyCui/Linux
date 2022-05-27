#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

struct thrd{
    int var;
    char str[256];
};


void *tfn(void *arg){ // 子线程

    struct thrd *tval;
    
    tval = (struct thrd*)malloc(sizeof(tval));

    tval->var = 100;
    strcpy(tval->str, "hello thread");

    return (void*)tval;
}

int main(){ //主线程

    pthread_t tid;
    struct thrd *retval;

    int ret = pthread_create(&tid,  NULL, tfn, NULL);
    if(ret != 0){
        perror("pthread_create error");
    }

    ret = pthread_join(tid, (void**)&retval);
    if(ret != 0)
        perror("pthread_join error");

    printf("child pthread exit with var = %d, str = %s\n", retval->var, retval->str );    
    pthread_exit(NULL);

}