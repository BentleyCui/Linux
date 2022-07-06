## 线程的基本概念

LWP： light weight process 轻量级的进程，本质仍是进程(在 Linux 环境下)  

**进程**

独立地址空间，拥有 PCB  

**线程**

有独立的 PCB，但没有独立的地址空间(共享)  

**区别：是否共享地址空间。  **

![image](https://user-images.githubusercontent.com/59153788/168845898-f1251474-024d-40b9-b64e-cf2194b689a7.png)

线程是最小的执行单位

进程是最小分配资源的单位

```shell
ps -Lf 进程id
查看线程号 LWP,cpu执行的最小单位
```

​    

## Linux内核线程实现原理

1. 轻量级进程(light-weight process)，也有 PCB，创建线程使用的底层函数和进一样，都是 clone  
2. 从内核里看进程和线程是一样的，都有各自不同的 PCB，但是 PCB 中指向内存源的三级页表是相同的 
3. 进程可以蜕变成线程
4. 线程可看做寄存器和栈的集合     

三级映射：进程 PCB --> 页目录(可看成数组，首地址位于 PCB 中) --> 页表 --> 物理页面 --> 内存单元  

对于进程来说，相同的地址(同一个虚拟地址)在不同的进程中，反复使用而不冲突。原因是他们虽虚拟址一样，但，页目录、页表、物理页面各不相同。相同的虚拟址，射到不同的物理页面内存单元，最终访问不同的物理页面。

但！线程不同！两个线程具有各自独立的 PCB，但共享同一个页目录，也就共享同一个页表和物理页面。所以两个 PCB 共享一个地址空间。

实际上，无论是创建进程的 fork，还是创建线程的 pthread_create，底层实现都是调用同一个内核函数 clone。  

**Linux 内核是不区分进程和线程的。只在用户层面上进行区分。所以，线程所有操作函数 pthread_\* 是库函数，而非系统调用**

​    

****

## 线程共享

**共享资源：**

1. 文件描述符表
2. 每种信号的处理方式
3. 当前工作目录
4. 用户 ID 和组 ID
5. 内存地址空间 (.text/.data/.bss/heap/共享库) 

**非共享资源：**

1. 线程id

2. 处理器现场和栈指针(内核栈)  

3. 独立的栈空间(用户空间栈)

4. errno 变量

5. 信号屏蔽字

6. 调度优先级  

   ​    

**线程间共享全局变量**

【牢记】：线程默认共享数据段、代码段等地址空间，常用的是全局变量。而进程不共享全局变量，只能借助 mmap。  

[pthread_glb_var.cpp](https://github.com/BentleyCui/Linux/blob/main/%E7%BA%BF%E7%A8%8B/pthread_glb_var.cpp)

​    

## 线程控制原语

#### pthread_self()

获取线程id，是在进程地址内部，用来标识线程身份的id

```c
 pthread_t pthread_self(void);
```

​    

#### pthread_create()

[pthread_create.cpp](https://github.com/BentleyCui/Linux/blob/main/%E7%BA%BF%E7%A8%8B/pthread_create.cpp)

```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg);
返回值：
    成功：0
    失败：errno
参数：
    thread：传出参数，新创建的子线程id
    attr：线程属性。传NULL表使用默认属性
    (void *)：子线程回调函数。创建成功，pthread_create返回时，该函数自动调用
    arg:回调函数的参数
```

在一个线程中调用 pthread_create()创建新的线程后，当前线程从 pthread_create()返回继续往下执行，而新的线
程所执行的代码由我们传给 pthread_create 的函数指针 start_routine 决定

**注意， pthread_exit 或者 return 返回的指针所指向的内存单元必须是全局的或者是用 malloc 分配的，不能在线程函数的栈上分配，因为当其它线程得到这个返回指针时线程函数已经退出了。  **

​    

#### 练习

循环创建多个线程，每个线程打印自己是第几个被创建的线程。 (类似于进程循环创建子进程)  

[pthread_more.c](https://github.com/BentleyCui/Linux/blob/main/%E7%BA%BF%E7%A8%8B/pthread_more.c)

​    

#### pthread_exit()

```c
void pthread_exit(void *retval);
retval：
    退出值
    无退出值时：NULL
```

退出**当前线程**

结论：线程中，禁止使用 exit 函数，会导致进程内所有线程全部退出。

[pthread_exit.cpp](https://github.com/BentleyCui/Linux/blob/main/%E7%BA%BF%E7%A8%8B/pthread_exit.cpp)

​    

#### pthread_join()

阻塞等待线程退出，获取线程退出状态  

其作用，对应进程中 waitpid() 函数。  

```c
int pthread_join(pthread_t thread, void **retval);
返回值：
    成功：0
    失败：错误号
```

[pthread_join.cpp](https://github.com/BentleyCui/Linux/blob/main/%E7%BA%BF%E7%A8%8B/pthread_join.cpp)

​    

#### pthread_cancle()

杀死(取消)线程 其作用，对应进程中 kill() 函数。  

```c
int pthread_cancel(pthread_t thread)
返回值：
    成功：0
    失败：错误号
```

线程的取消并不是实时的，而有一定的延时。需要等待线程到达某个取消点(检查点)。  

可粗略认为一个系统调用(进入内核)即为一个取消点。如线程中没有取消点，可以通过调用pthread_testcancel函数自行设置一个取消点 。

成功被 pthread_cancel( ) 杀死的线程，返回 -1，使用 pthread_join 回收

[pthread_cancel.cpp](https://github.com/BentleyCui/Linux/blob/main/%E7%BA%BF%E7%A8%8B/pthread_cancel.cpp)

​      

#### pthread_detach()

实现线程分离

一般情况下，线程终止后，其终止状态一直保留到其它线程调用 pthread_join 获取它的状态为止。但是线程也 可以被置为 detach 状态，**这样的线程一旦终止就立刻回收它占用的所有资源，而不保留终止状态。**

```c
int pthread_detach(pthread_t thread)
```

**线程分离状态**：指定该状态，线程主动与主控线程断开关系。线程结束后，其退出状态不由其他线程获取，而直接自己自动释放。网络、多线程服务器常用。  

进程若有该机制，将不会产生僵尸进程。僵尸进程的产生主要由于进程死后，大部分资源被释放，一点残留资 源仍存于系统中，导致内核认为该进程仍存在。

****

#### 终止线程的三种方式

1. 从线程主函数 return。这种方法对主控线程不适用，从 main 函数 return 相当于调用 exit
2. 一个线程可以调用 pthread_cancel 终止同一进程中的另一个线程
3. 线程可以调用 pthread_exit 终止自己。

****

#### 控制原语对比



![image](https://user-images.githubusercontent.com/59153788/169250971-f4401494-7113-4b0b-9498-ea532b7643b9.png)



​     

