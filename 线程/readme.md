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

```c
int pthread_detach(pthread_t thread)
```

线程分离状态：指定该状态，线程主动与主控线程断开关系。线程结束后，其退出状态不由其他线程获取，而
直接自己自动释放。网络、多线程服务器常用。  

#### 控制原语对比



![image](https://user-images.githubusercontent.com/59153788/169250971-f4401494-7113-4b0b-9498-ea532b7643b9.png)



​     

## 线程同步

“同步”的目的，是为了避免数据混乱，解决与时间有关的错误  

#### 数据混乱的原因

1. 资源共享
2. 调度随机（线程竞争）
3. 线程间缺乏必要的同步机制

​    

## 互斥量mutex



![image](https://user-images.githubusercontent.com/59153788/169273385-281b11cf-1254-42e0-9d4d-06e962d12d21.png)

互斥锁实质上是操作系统提供的一把“建议锁”（又称“协同锁”），建议程序中有多线程访问共享资源的时候使用该机制。但，并没有强制限定。  

[pthread_mutex.cpp](https://github.com/BentleyCui/Linux/blob/main/%E7%BA%BF%E7%A8%8B/pthread_mutex.cpp)



#### 锁的使用

建议锁！对共享数据进行保护。所有线程应该在访问共享数据前先拿锁再访问，但锁本身不具备强制性



![image](https://user-images.githubusercontent.com/59153788/170617212-7131edb7-8bff-49e4-a0ac-835b1b01278c.png)

 **注意事项：**尽量保证锁的粒度，越小越好。访问共享数据前加锁，访问结束后立即解锁

互斥锁，本质是结构体。可以看出整数，初值为1

加锁：--操作，阻塞线程

解锁：++操作，唤醒阻塞在锁上的线程

try锁：以非阻塞方式尝试加锁 



## 死锁

不是锁，是使用锁不恰当导致的现象:

1. 对同一个互斥量重复加锁
2. 循环等待对方持有的锁

避免方法：

1. 保证资源的获取顺序，要求每个线程获取资源的顺序一致
2. 当得不到所有所需的资源时，放弃已经获得的资源

## 读写锁

1. 读共享，写独占
2. **写锁优先级高**
3. 锁只有一把

类型： pthread_rwlock_t;

[pthread_rwlock.c](https://github.com/BentleyCui/Linux/blob/main/%E7%BA%BF%E7%A8%8B/pthread_rwlock.c)  

  

## 条件变量

条件变量本身不是锁！但它也可以造成线程阻塞。通常与互斥锁配合使用。  

pthread_cond_init 函数  

pthread_cond_destroy 函数  

pthread_cond_wait 函数， 等待条件满足

pthread_cond_timedwait 函数  

pthread_cond_signal 函数 ：**唤醒阻塞在条件变量上的一个线程** 

pthread_cond_broadcast 函数  ：**广播方式唤醒阻塞在条件变量上的所有线程**



#### pthread_cond_wait()

```c
int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);
```

功能：

1. 阻塞等待条件变量 cond（参 1）满足
2. 释放已掌握的互斥锁（解锁互斥量） 相当于 pthread_mutex_unlock(&mutex);  
3. 当被唤醒， pthread_cond_wait 函数返回时，解除阻塞并重新申请获取互斥锁 pthread_mutex_lock(&mutex);    

![image](https://user-images.githubusercontent.com/59153788/170939176-bcfd8ebe-6b2a-4330-baea-1e6bb42f9a10.png)



## 条件变量的生产者消费者模型



![image](https://user-images.githubusercontent.com/59153788/170942537-edbc6e6e-37a9-4fdb-bc27-93259c423ca7.png)



​    

借助条件变量实现生产者消费者模型

[conditionVar_product_consumer.cpp](https://github.com/BentleyCui/Linux/blob/main/%E7%BA%BF%E7%A8%8B/conditionVar_product_consumer.cpp)

## 信号量

**进化版本的互斥锁**

相当于 初始化值 为N 的互斥量

允许同时有 N个线程 同时访问共享资源

sem_init 函数

sem_destroy 函数

sem_wait 函数

sem_trywait 函数

sem_timedwait 函数

sem_post 函数  ：解锁

sem_t类型，头文件<semaphore.h>

**既可以用在线程，又可以用在进程**

```c
sem_t sem;
int sem_init(sem_t *sem, int pshared, unsigned int value);
参数：
    pshared 取 0 用于线程间；取非 0（一般为 1）用于进程间
    value 指定信号量初值
sem_init(&sem, 0, 8);
int sem_wait(sem_t *sem);
// 完成信号量的加锁
int sem_post(sem_t *sem);
// 解锁
```



​    

