
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>

#include "tcp_pro_comm.h"
#include "net_comm.h"



int tcp_create_sock()
{
	int fd = -1;

	fd = net_create_sock(AF_INET, SOCK_STREAM);
	if (fd < 0)
	{
		pperror("net_create_sock: error! return = %d\n", fd);
		return -__LINE__;
	}
	return fd;
}

int tcp_create_nonblock_sock()
{
	int fd = -1;
	fd = tcp_create_sock();
	if (fd < 0)
	{
		pperror("tcp_create_sock error! return = %d\n", fd);
		return -__LINE__;
	}
	int iRet = -1;
	iRet = net_sock_set_nonblock(fd);
	if (iRet < 0)
	{
		pperror("net_sock_set_nonblock error! return = %d\n", iRet);
		return -__LINE__;
	}
	return fd;
}

int tcp_sock_nsend(const int fd, const char *data, const int len)
{
	int  nwrite = 0;
	int  i = 0;

	while(i < len)
	{
		do{
			nwrite = write(fd, data+i, len - i);
		} while((nwrite < 0) && (errno == EINTR));
		if(nwrite <= 0)
		{ 
			return nwrite;
		}
		i += nwrite;
	}
	return len;
}

int tcp_sock_accept(int fd, void * addr){
	struct sockaddr_in *paddr, locAddr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	paddr = (struct sockaddr_in *)addr;
	if (NULL == addr){
		paddr = &locAddr;
	}

	int cltfd = accept(fd, (struct sockaddr *)paddr, &addrlen);
	if (cltfd < 0)
	{
		pperror("accept error! : %s\n", strerror(errno));
	}

	return cltfd;
}

int tcp_sock_nrecv(const int fd, char *data, int len)
{

	int  nread = 0;
	int i=0;

	while(nread < len)
	{
		do {
			nread = read(fd, data+i, len - i);
		} while((nread < 0) && (errno == EINTR));
		if(nread < 0)
		{
			return nread;
		} else if(nread == 0) {
			return i;
		}
		i += nread;
	}
	return len;
}

int tcp_create_and_connect(const char *ip, int port)
{
	int fd = -1;
	fd = tcp_create_sock();
	if (fd < 0){
		return -__LINE__;
	}

	int iRet = 0;
	iRet = net_sock_connect(fd, ip, port);
	if (iRet < 0){
		pperror("net_sock_connect error! return = %d\n", iRet);
		return -__LINE__;
	}
	return fd;
}

int tcp_sock_listen(const int fd, int backlog)
{
	if (listen(fd, backlog) == -1){
		pperror("listen error! : %s\n", strerror(errno));
		return -__LINE__;
	}
	return 0;
}

int tcp_create_and_listen(const char *ip, int port, int backlog)
{
	
	int fd = -1;
	fd = tcp_create_sock();
	if (fd < 0){
		pperror("tcp_create_sock error! return = %d\n", fd);
		return -__LINE__;
	}
	int iRet = 0;
	iRet = tcp_sock_bind(fd, ip, port);
	if (iRet < 0){
		pperror("tcp_sock_bind error! return = %d\n", fd);
		return -__LINE__;
	}

	iRet = tcp_sock_listen(fd, backlog);
	if (iRet < 0){
		pperror("tcp_sock_listen error! return = %d\n", fd);
		return -__LINE__;
	}

	return fd;
}


int tcp_sock_connect(int fd, const char *ip, int port){
	return net_sock_connect(fd, ip, port); 
}

int tcp_sock_bind(int fd, const char *ip, int port){
	return net_sock_bind(fd, ip, port);
}

int tcp_sock_timeout_recv(int fd, char *data, int *len, int utimeout){
	return net_sock_timeout_recv(fd, data, len, utimeout);
}

