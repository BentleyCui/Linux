#### fork函数

```c
pit_d fork(void)
```

功能：创建子进程；父子进程各自返回，父进程返回Pid, 子进程返回0

```c
getpid();
getppid();
```

#### 循环创建N个子进程模型

代码： fork_loop.cpp

每个子进程标识自己的身份

#### 父子进程共享哪些内容

刚fork() 之后：

​	父进程与子进程**相同之处**：全局变量、.data、.text、栈、堆、环境变量、用户ID、进程工作目录等

​	**不同之处**：1.进程ID    2. fork 返回值     3. 父进程ID      4. 进程运行时间     5. 闹钟    6. 未决信号集

**注意！躲避父子进程共享全局变量的知识误区**：当子进程与父进程读取全局变量时，是共享。当子进程或父进程写全局变量时，是复制。

​	**父子进程共享：1. 文件描述符    2. mmap建立的映射区**

​	父子进程之间遵循**读时共享，写时复制**的原则

**特别的，fork之后父进程先执行还是子进程先执行不确定，取决于内核所使用的调度算法**