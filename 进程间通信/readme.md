### IPC ( InterProcess Communication )

Linux 环境下，进程地址空间相互独立，每个进程各自有不同的用户地址空间。任何一个进程的全局变量在另一个进程中都看不到，所以进程和进程之间不能相互访问，要交换数据必须通过内核，在内核中开辟一块缓冲区，进程 1 把数据从用户空间拷到内核缓冲区，进程 2 再从内核缓冲区把数据读走，内核提供的这种机制称为进程间通信（IPC， InterProcess Communication）。  

**本质：内核空间中的一块缓冲区**

![image](https://user-images.githubusercontent.com/59153788/167784041-dee74ba8-0eed-4bc1-9edb-b9c03298a29b.png)

现今常用的进程间通信方式有：  

**（1）管道**（简单）

**（2）信号**（开销最小）

**（3）共享映射区**（没有血缘关系的）

**（4）本地套接字**（最稳定）



### 管道

管道是一种最基本的 IPC 机制， **作用于有血缘关系的进程**之间，完成数据传递。调用 pipe 系统函数即可创建一个管道。  

有如下特质：

1. 其本质是一个**伪文件**(实为内核缓冲区)
2. 由两个文件描述符引用，一个表示**读端**，一个表示**写端**。  
3. 规定数据从管道的写端流入管道，从读端流出。  

管道的原理: 管道实为内核使用**环形队列**机制，借助内核缓冲区(4k)实现。  

**局限性：**

1. 数据不能自己写，自己读。  
2. 管道中数据**不可反复读取**。一旦读走，管道中不再存在。  
3. 采用**半双工通信**方式，数据只能在单方向上流动。  



### pipe函数

创建，并打开管道

```c
int pipe(int pipefd[2]);
参数：
    fd[0]:读端
    fd[1]:写端
返回值：
    成功：0
    失败：-1，设置errno
```

管道创建成功以后， 创建该管道的进程（父进程） 同时掌握着管道的读端和写端。  

<img src="https://user-images.githubusercontent.com/59153788/167789156-f243f32a-2f41-40f3-8e45-7c9b72293c26.png" alt="image" style="zoom: 80%;" />

1. 父进程调用 pipe 函数创建管道，得到两个文件描述符 fd[0]、 fd[1]指向管道的读端和写端。
2. 父进程调用 fork 创建子进程，那么子进程也有两个文件描述符指向同一管道。  
3. 父进程关闭管道读端，子进程关闭管道写端。父进程可以向管道中写入数据，子进程将管道中的数据读出。由于管道是利用环形队列实现的，数据从写端流入管道，从读端流出，这样就实现了进程间通信。  



  [pipe创建管道进行通信 pipe.cpp](https://github.com/BentleyCui/Linux/blob/main/%E8%BF%9B%E7%A8%8B%E9%97%B4%E9%80%9A%E4%BF%A1/pipe.cpp)



### 管道的读写行为

1. **读管道：**
   1. 管道中有数据：read 返回实际读到的字节数  
   2. 管道中无数据：
      1. 管道写端被全部关闭：read 返回 0 (好像读到文件结尾)  
      2. 写端没有全部被关闭：read 阻塞等待(不久的将来可能有数据递达，此时会让出 cpu)  
2. **写管道：**
   1. 管道读端全部被关闭， 进程异常终止(也可使用捕捉SIGPIPE 信号，使进程不终止)  
   2. 管道读端没有全部关闭：
      1. 管道已满：write阻塞
      2. 管道未满：write 将数据写入，并返回实际写入的字节数  

**练习：**

使用管道实现父子进程间通信，完成： ls | wc –l。 假定父进程实现 ls， 子进程实现 wc。

ls 命令正常会将结果集写出到 stdout，但现在会写入管道的写端； wc –l 正常应该从 stdin 读取数据， 但此时会从管道的读端读。  

[pipe_lswc.cpp](https://github.com/BentleyCui/Linux/blob/main/%E8%BF%9B%E7%A8%8B%E9%97%B4%E9%80%9A%E4%BF%A1/pipe_lswc.cpp)

```c
pipe_lswc.cpp
功能：实现 ls | wc -l 命令
```



使用管道进行兄弟子进程间的通信：

[pipe_brother.cpp](https://github.com/BentleyCui/Linux/blob/main/%E8%BF%9B%E7%A8%8B%E9%97%B4%E9%80%9A%E4%BF%A1/pipe_brother.cpp)

注意：需要关闭父进程的读写端，保证数据的单向流动



###  FIFO 命名管道

管道(pipe)只能用于“有血缘关系”的进程间。但通过 FIFO，不相关的进程也能交换数据。

FIFO 是 Linux 基础**文件类型**中的一种。但， FIFO 文件在磁盘上没有数据块，仅仅用来标识内核中一条通道。各进程可以打开这个文件进行 read/write， 实际上是在读写内核通道，这样就实现了进程间通信。    

**创建命令**：

（1）命令：**mkfifo**

（2）库函数： int mkfifo(const char *pathname, mode_t mode);   成功： 0； 失败： -1  

一旦使用 mkfifo 创建了一个 FIFO， 就可以使用 open 打开它， 常见的文件 I/O 函数都可用于 fifo。 如： close、 read、write、 unlink 等。  

```c
fifo_write.cpp
fifo_read.cpp
    
    一个写端，一个读端
    测试一个写端多个读端的时候，由于数据一旦被读走就没了，所以多个读端的并集才是写端的写入数据
```

[fifo_write.cpp](https://github.com/BentleyCui/Linux/blob/main/%E8%BF%9B%E7%A8%8B%E9%97%B4%E9%80%9A%E4%BF%A1/fifo_write.cpp)

[fifo_read.cpp](https://github.com/BentleyCui/Linux/blob/main/%E8%BF%9B%E7%A8%8B%E9%97%B4%E9%80%9A%E4%BF%A1/fifo_read.cpp)



### 文件用于进程通信

使用文件也可以完成 IPC，理论依据是， fork 后，父子进程共享文件描述符。也就共享打开的文件。  

在父子进程的PCB中，文件描述符表相同，指向同一文件

无血缘关系的两个进程也可以通过文件进行进程通信



### 存储映射 I/O  

存储映射 I/O (Memory-mapped I/O) 使一个磁盘文件与存储空间中的一个缓冲区相映射。 于是当从缓冲区中取数据， 就相当于读文件中的相应字节。  这样，就可在不适用 read 和 write 函数的情况下，使用地址（指针）完成 I/O 操作。  

使用这种方法， 首先应通知内核， 将一个指定文件映射到存储区域中。 这个映射工作可以通过 mmap 函数来实
现。  

**与管道不同，可以重复读**

<img src="https://user-images.githubusercontent.com/59153788/168009252-05006e5a-c15b-4422-be9e-a7513ed1db74.png" alt="image" style="zoom:80%;" />

#### mmap函数

功能：创建**共享内存映射**

```c
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
参数：
	addr: 建立映射区的首地址，由 Linux 内核指定。使用时，直接传递 NULL
    length: 欲创建映射区的大小  (<=文件的实际大小)
    prot: 映射区权限 PROT_READ、 PROT_WRITE、 PROT_READ|PROT_WRITE
    flags: 标志位参数(标准共享内存的共享属性)
			MAP_SHARED: 会将映射区所做的操作反映到物理设备（磁盘）上。
			MAP_PRIVATE: 映射区所做的修改不会反映到物理设备。
    fd: 用来建立映射区的文件描述符
    offset: 映射文件的偏移(4k 的整数倍)；默认0表示映射文件全部
        
返回值：
        成功：返回创建的映射区首地址；
        失败： MAP_FAILED 宏 ((void*) -1)，并设置 errno
```

释放映射区

同 malloc 函数申请内存空间类似的， mmap 建立的映射区在使用结束后也应调用类似 free 的函数来释放  

```c
int munmap(void *addr, size_t length);
成功返回0，失败返回-1
```

[mmap.cpp](https://github.com/BentleyCui/Linux/blob/main/%E8%BF%9B%E7%A8%8B%E9%97%B4%E9%80%9A%E4%BF%A1/mmap.cpp)



#### mmap注意事项

1. 用于创建映射区的文件大小为 0，实际指定非0大小创建映射区，出 “总线错误”。

2. 用于创建映射区的文件大小为 0，实际指定0大小创建映射区， 出 “无效参数”。

3. 用于创建映射区的文件读写属性为，只读。映射区属性为 读、写。 出 “无效参数”。

4. offset 必须是 4096的整数倍。（MMU 映射的最小单位 4k ）

5. munmap用于释放的 地址，必须是mmap申请返回的地址。




#### mmap父子进程通信

   父子等有血缘关系的进程之间也可以通过 mmap 建立的映射区来完成数据通信。但相应的要在创建映射区的时
候指定对应的标志位参数 flags：  

**MAP_SHARED**：父子进程共享映射区

练习：父进程创建映射区，然后 fork 子进程，子进程修改映射区内容，而后，父进程读取映射区内容，查验是
否共享  

[mmap_parent.cpp](https://github.com/BentleyCui/Linux/blob/main/%E8%BF%9B%E7%A8%8B%E9%97%B4%E9%80%9A%E4%BF%A1/mmap_parent.cpp)

父进程先创建映射区，fork()后，父子进程都拥有该映射区的地址



#### mmap无血缘关系的进程间通信

实质上 mmap 是内核借助文件帮我们创建了一个映射区，多个进程之间利用该映射区完成数据传递。由于内核
空间多进程共享，因此无血缘关系的进程间也可以使用 mmap 来完成通信。  

[mmap_write.cpp](https://github.com/BentleyCui/Linux/blob/main/%E8%BF%9B%E7%A8%8B%E9%97%B4%E9%80%9A%E4%BF%A1/mmap_write.cpp)

[mmap_read.cpp](https://github.com/BentleyCui/Linux/blob/main/%E8%BF%9B%E7%A8%8B%E9%97%B4%E9%80%9A%E4%BF%A1/mmap_read.cpp)

【注意】无血缘关系进程间通信：

**mmap：** 数据可以重复读

**fifo：**数据只能读一次



#### 匿名映射

通过使用我们发现， 使用映射区来完成文件读写操作十分方便， 父子进程间通信也较容易。 但缺陷是， 每次创
建映射区一定要依赖一个文件才能实现。通常为了建立映射区要 open 一个 temp 文件，创建好了再 unlink、close掉，比较麻烦。 可以直接使用匿名映射来代替。 其实 Linux 系统给我们提供了创建匿名映射区的方法， 无需依赖一个文件即可创建映射区。 同样需要借助标志位参数 flags 来指定。  

使用 MAP_ANONYMOUS (或 MAP_ANON)， 如:  

```c
int *p = mmap(NULL, 4, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
```

[mmap_anon.cpp](https://github.com/BentleyCui/Linux/blob/main/%E8%BF%9B%E7%A8%8B%E9%97%B4%E9%80%9A%E4%BF%A1/mmap_anon.cpp)







​    
