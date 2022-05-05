#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
/*
    read():
    返回值：
        0：读到文件末尾
        成功：返回读到的字节数
        失败： -1，设置errno
        -1:如果errno == EAGAIN | EWOULDBLOCK，说明不是read失败，而是以非阻塞方式读取一个设备文件（网络文件），并且文件无数据
*/
#define MSG_TIMEOUT "time out\n"

int main(){
    char buf[10];
    int fd, n;

    // /dev/tty 对应的是终端文件描述符
    fd = open("/dev/tty", O_RDONLY|O_NONBLOCK); //指定非阻塞属性
    if(fd < 0){
        perror("open /dev/tty");
        exit(1);
    }
    printf("open /dev/tty OK...%d\n", fd);
    //read()读取非阻塞文件时，若没有读到数据 返回-1，并设置err为EAGAIN 或 EWOULDBLOCK
    int i;
    for(i=0; i<5; i++){
        n = read(fd, buf, 10);
        if(n > 0){
            break;
        }
        if(errno != EAGAIN){ 
            perror("read /dev/tty");
            exit(1);

        }else{
            write(STDOUT_FILENO, "try again\n", strlen("try again\n")); //标准输出
            sleep(2); 
        }
    }

    if(i == 5){
        write(STDOUT_FILENO, MSG_TIMEOUT, strlen(MSG_TIMEOUT));
    }else{
        write(STDOUT_FILENO, buf, n);
    }

    close(fd);

    return 0;
}