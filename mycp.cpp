#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

// 使用read()和write()系统调用实现 cp指令
// cp text1 text2

int main(int argc, char* argv[]){
    
    int fd1 = open(argv[1], O_RDONLY);   //read
    
    int fd2 = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0664); //不存在该文件的话，创建；存在的话，截断；

    int n = 0;
    char buf[1024];
    // 一次读取n个字节, read()==0表示读到文件末尾

    while( (n = read(fd1, buf, 1024) ) != 0){
        write(fd2, buf, n);
    }

    close(fd1);
    close(fd2);
    return 0;

}