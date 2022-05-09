#### exec函数族

fork（）创建子进程后执行的是和父进程相同的程序，子进程往往要调用一种 exec 函数以执行另一个程序

当进程调用一种exec函数时，该进程的用户空间代码和数据完全被新程序替换，从新程序的启动例程开始执行。

**调用exec函数不创建新的进程，所以进程号pid并未改变**

![image](https://user-images.githubusercontent.com/59153788/167384181-b6da1a66-b456-4647-a1f1-785e0738f1e8.png)

```c
int execl(const char *path, const char *arg, ...); // path为本地的进程
int execlp(const char *file, const char *arg, ...); // 加载一个进程，借助PATH环境变量
// 例如
execlp("ls", "ls", "-l", "-a", NULL); // 执行 ls -la 命令
execl("./a.out", "./a.out", NULL); // 子进程执行本地a.out程序
execl("/bin/ls", "ls", "-l", NULL);
返回值：只有当出错时才返回-1
```

**函数：fork_exec.cpp**

