#include<stdio.h>
#include <unistd.h>

/**
    测试计算机一秒能打印多少数字
    实际执行时间 = 系统时间 + 用户时间 + 等待时间
**/
int main(){
    int i;
    alarm(1);

    for(i=0; ;i++)
        printf("%d\n", i);

    return 0;
}