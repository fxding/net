## 说明
这个 tcp/udp 库是一个简单的工具库. 你可以直接通过每个函数的名字中知道他的作用, 比如说`int tcp_create_nonblock_sock()` , 你可以通过这个函数创建一个非阻塞的 tcp 套接字.<br/>

这个库的设计思想是:<br/>
+ 1. 可以直接通过名字来直观的调用底层的网络接口.
+ 2. 将底层接口按不同的需求组合在一起, 使其可以通过一次调用达到多个目的
+ 3. 屏蔽底层细节, 比如说要创建一个 `tcp socket`, 你不必知道`AF_INET` 或者 `SOCK_STREAM` 是什么东西.

## 文件
net_comm.* : tcp/udp 共有的基础函数库 <br/>
tcp_pro_comm.* : tcp 特定的函数库 <br/>
udp_pro_comm.* : udp 特定的函数库 <br/>

使用上面库编写的tcp/udp echo server.<br/>
tcp_echo_client.c/tcp_echo_server.c : tcp echo server 和 client <br/>
udp_echo_client.c/udp_echo_server.c : udp echo server 和 client <br/>

## tips

+ 1. 创建一个监听某个端口的 `socket` 你可以使用下面两种方式.<br/>

```
int fd = -1;
fd = tcp_create_sock();
tcp_sock_bind(fd, 0, 201349);
tcp_sock_listen(fd, 10);
```

```
fd = tcp_create_and_listen(0, 201349, 10);
```
