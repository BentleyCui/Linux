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

 

​    

