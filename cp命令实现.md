### cp命令实现    
**命令格式：**    
        cp test1.txt  test2.txt    
    mycp.cpp通过read()和write()函数实现 cp 命令     
    read、 write 函数常常被称为 Unbuffered I/O。 指的是无用户级缓冲区。 但不保证不使用内核
缓冲区
