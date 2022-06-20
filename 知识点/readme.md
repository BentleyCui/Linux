## 系统调用

由操作系统实现并提供给外部应用程序的编程接口，是应用程序同系统之间数据交互的桥梁。

![image](https://user-images.githubusercontent.com/59153788/174609008-82c84f7d-9627-4e34-beab-add3214470c4.png)

## 文件描述符

### [文件描述符表](https://github.com/BentleyCui/Linux/blob/main/%E7%9F%A5%E8%AF%86%E7%82%B9/%E6%96%87%E4%BB%B6%E6%8F%8F%E8%BF%B0%E7%AC%A6%E8%A1%A8.md)

![image](https://user-images.githubusercontent.com/59153788/174609421-7086380a-d798-46e3-906a-63808359331d.png)

* 结构体 PCB 的成员变量 file_struct *file 指向文件描述符表。
* 从应用程序使用角度，该指针可理解记忆成一个字符指针数组，下标 0/1/2/3/4...找到文件 结构体。
* 本质是一个键值对 0、1、2...都分别对应具体地址。但键值对使用的特性是自动映射，我 们只操作键不直接使用值。
* 新打开文件返回文件描述符表中未使用的最小文件描述符。

### file结构体

主要包含文件描述符、文件读写位置、IO缓冲区三部分内容

## read/write函数

```c
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
```

## 阻塞、非阻塞


