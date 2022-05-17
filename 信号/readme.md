# 信号

******

## 信号的机制

A 给 B 发送信号， B 收到信号之前执行自己的代码，**收到信号后，不管执行到程序的什么位置，都要暂停运行，去处理信号，处理完毕再继续执行**。与硬件中断类似——**异步模式**。但信号是软件层面上实现的中断，早期常被称为“软中断”。

**信号的特质：**由于信号是通过**软件方法**实现，其实现手段导致信号有很强的延时性。但对于用户来说，这个延迟时间非常短，不易察觉。    

**所有信号的产生和处理都是由内核负责的**

******

## 与信号相关的事件和状态  

**产生信号**：

1. 按键产生
2. 系统调用产生，如：kill
3. 软件条件产生，如：定时器 alarm  
4. 硬件异常产生，如：非法访问内存(段错误)、除 0(浮点数例外)、内存对齐出错(总线错误)  
5. 命令产生，kill

​    

**递达**：递送并且到达进程。 直接被内核处理

**未决**：产生和递达之间的状态。主要由于阻塞(屏蔽)导致该状态。

​      

**信号的处理方式：**

1. 执行默认动作  ：每一个信号都有属于自己的默认处理动作
2. 忽略(丢弃)  
3. 捕捉(调用户处理函数)   ：自定义

Linux 内核的进程控制块 PCB 是一个结构体， task_struct, 除了包含进程 id，状态，工作目录，用户 id，组 id，文件描述符表，还包含了**信号相关的信息**，主要指阻塞信号集和未决信号集。  

​    

**阻塞信号集(信号屏蔽字)：** 将某些信号加入集合，对他们设置屏蔽，当屏蔽 x 信号后，再收到该信号，该信号的处理将推后(解除屏蔽后)  

**未决信号集：**

1. 信号产生，未决信号集中描述该信号的位立刻翻转为 1，表信号处于未决状态。当信号被处理对应位翻转回为 0。这一时刻往往非常短暂。  
2. 信号产生后由于某些原因(主要是阻塞)不能抵达。这类信号的集合称之为未决信号集。在屏蔽解除前，信号一直处于未决状态。  



<img src="https://user-images.githubusercontent.com/59153788/168221584-da6b8896-0114-4d50-b213-c004be7a5df5.png" alt="image" style="zoom:80%;" />

**信号的执行流程：**以 Ctrl - C 为例

1. 按键 Ctrl - C ，代表2号信号
2. 未决信号集的2号置位为1，发送信号。有三种处理方式
3. 若此时阻塞信号集对应位为1，则该信号被屏蔽，一直处于未决态。直到信号屏蔽字为0，解除阻塞

​    

****

## 信号四要素

**信号使用前，应先确定信号四要素**

1. 编号
2. 名称
3. 信号对应的事件
4. 信号默认处理动作

```c
man 7 signal
```

**默认动作：**
Term：终止进程
Ign： 忽略信号 (默认即时对该种信号忽略操作)
Core：终止进程，生成 Core 文件。 (查验进程死亡原因， 用于 gdb 调试)
Stop：停止（暂停）进程
Cont：继续运行进程  

​    

****

### kill函数

[kill.cpp](https://github.com/BentleyCui/Linux/blob/main/%E4%BF%A1%E5%8F%B7/kill.cpp)

给指定进程发送指定信号(不一定杀死)   

```c
int kill(pid_t pid, int sig);
返回值：
	成功：0
	失败：-1，设置errno
sig：不推荐直接使用数字，应使用宏名，因为不同操作系统信号编号可能不同，但名称一致

pid > 0: 发送信号给指定的进程。
pid = 0: 发送信号给 与调用 kill 函数进程属于同一进程组的所有进程，父子进程属于同一进程组
pid < 0: 取|pid|发给对应进程组
pid = -1：发送给进程有权限发送的系统中所有进程。
    
```

进程组：每个进程都属于一个进程组，进程组是一个或多个进程集合，他们相互关联，共同完成一个实体任务，每个进程组都有一个进程组长，默认进程组 ID 与进程组长 ID 相同。

​    

****

### 软件条件产生信号

#### alarm函数

设置定时器(闹钟)。在指定 seconds 后，内核会给当前进程发送   14） **SIGALRM 信号**。进程收到该信号，**默认动作终止**。  

**每个进程都有且只有唯一个定时器。  **

```c
unsigned int alarm(unsigned int seconds);
返回 0 或剩余的秒数，无失败情况。0表示没有定时
常用：取消定时器 alarm(0)， 返回旧闹钟余下秒数
```

实际执行时间 = 系统时间 + 用户时间 + 等待时间  

**time命令**：查看程序执行时间

[alarm.cpp](https://github.com/BentleyCui/Linux/blob/main/%E4%BF%A1%E5%8F%B7/alarm.cpp)

​        

#### setitimer函数

设置定时器(闹钟)。 可代替 alarm 函数。精度微秒 us，可以实现周期定时。  

```c
#include <sys/time.h>
int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);
返回值：
    成功： 0；
    失败： -1，设置errno
参数:
	which：指定定时方式
        (1)自然定时： ITIMER_REAL → 14） SIGLARM  计算自然时间
        (2)虚拟空间计时(用户空间)： ITIMER_VIRTUAL → 26） SIGVTALRM 只计算进程占用 cpu 的时间
        (3)运行时计时(用户+内核)： ITIMER_PROF → 27） SIGPROF 计算占用 cpu 及执行系统调用的时间
    new_value:传入参数，新的定时时间
	old_value:传出参数，上次定时剩余时间
```

```c
struct itimerval {
       struct timeval it_interval; /* Interval for periodic timer */
       struct timeval it_value;    /* Time until next expiration */
};
struct timeval {
       time_t      tv_sec;         /* seconds */
       suseconds_t tv_usec;        /* microseconds */
};

it_interval：用来设定两次定时任务之间间隔的时间。
it_value：定时的时长
两个参数都设置为 0，即清 0 操作。
```

练习：测试 it_interval、 it_value 这两个参数的作用。[setitimer_cycle.cpp](https://github.com/BentleyCui/Linux/blob/main/%E4%BF%A1%E5%8F%B7/setitimer_cycle.cpp)

​    


