#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//以阻塞的方式read
int main(){
    char buf[10];

    int n;
    n = read(STDIN_FILENO, buf, 10); // STDIN_FILENO对应的文件描述符为0，标准输入；STDOUT_FILENO  1; STDERR_FILENO  2
    // 有数据的时候，读入。没有数据的时候，阻塞
    if(n<0){
        perror("read STDIN_FILENO");
        exit(1);
    }

    write(STDOUT_FILENO,buf,n);

    return 0;
}