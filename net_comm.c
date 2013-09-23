#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "net_comm.h"

int net_create_sock(int domain, int type)
{
	int fd = -1;
	fd = socket(domain, type, 0);
	if (-1 == fd){
		return -__LINE__;
	}
	int flag = 1;
	int iRet = 0;
	iRet = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
	if (-1 == iRet)
	{
		pperror("setsockopt: %s\n", strerror(errno));
		return -__LINE__;
	}
	return fd;
}

int net_sock_set_nonblock(int fd)
{
	int flags = 0;
	flags = fcntl(fd, F_GETFL);
	int iRet = 0;
	iRet = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	if (-1 == iRet)
	{
		pperror("fcntl: %s\n", strerror(errno));
		return -__LINE__;
	}
	return 0;
}

int net_sock_get_buffer(int fd, long *buflen, int type)
{
	socklen_t len = sizeof(*buflen);
	return getsockopt(fd, SOL_SOCKET, type, buflen, &len);
}

int net_sock_get_sndbuffer(int fd, long *buflen)
{
	return net_sock_get_buffer(fd, buflen, SO_SNDBUF);
}
int net_sock_get_rcvbuffer(int fd, long *buflen)
{
	return net_sock_get_buffer(fd, buflen, SO_RCVBUF);
}

int net_sock_set_buffer(int fd, long buflen, int type)
{
	int iRet = 0;
	if (SO_RCVBUF == type){
		iRet = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (void *) &buflen, sizeof(buflen));
		if (iRet < 0)
		{
			pperror("setsockopt: %s\n", strerror(errno));
			return -__LINE__;
		}
	} else if (SO_SNDBUF == type){
		iRet = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void*)&buflen, sizeof(buflen));
		if (iRet < 0)
		{
			pperror("setsockopt: %s\n", strerror(errno));
			return -__LINE__;
		}	
	}
	return 0;
}

int net_sock_set_sndbuffer(int fd, long buflen){
	return net_sock_set_buffer(fd, buflen, SO_SNDBUF);
}

int net_sock_set_rcvbuffer(int fd, long buflen){
	return net_sock_set_buffer(fd, buflen, SO_RCVBUF);
}

/* server */                                         
int net_sock_bind(int fd, const char *ip, int port)  
{                                           
	struct sockaddr_in sa;                 
	memset(&sa, 0, sizeof(struct sockaddr_in));    

	sa.sin_family = AF_INET;                      
	sa.sin_port   = htons(port);

	if (NULL == ip){
		sa.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		if (inet_aton(ip, &(sa.sin_addr)) == 0)
		{
			pperror("inet_aton: %s\n", strerror(errno));
			return -__LINE__;
		}
	}	

	if (bind(fd, (struct sockaddr*)&sa, sizeof(struct sockaddr_in)) == -1){      
		pperror("bind: %s\n", strerror(errno));
		return -__LINE__;                                     
	}                                                        
	return fd;                                              
}                                                          

/* client */                                          
int net_sock_connect(int fd, const char *ip, int port)          
{                                                        
	struct sockaddr_in sa;                              
	memset(&sa, 0, sizeof(struct sockaddr_in));        

	sa.sin_family = AF_INET;                          
	sa.sin_port   = htons(port);                     
	if (inet_aton(ip, &(sa.sin_addr)) == 0)        
	{                                              
		pperror("inet_ntoa: %s\n", strerror(errno));
		return -__LINE__;                         
	}                                           
	if (connect(fd, (struct sockaddr*)&sa, sizeof(struct sockaddr_in)) != 0) 
	{                                                     
		pperror("connect: %s\n", strerror(errno));
		return -__LINE__;                                
	}                                                   
	return fd;                                         
}

int net_sock_sendrecv(int fd, char *sdata, int slen, char *rdata, int *rlen, int utimeout)
{
	int iRet = 0;

	iRet = send(fd, sdata, slen, 0);
	if (iRet <=0 ){
		pperror("send: %s\n", strerror(errno));
		return -__LINE__;
	}

	iRet = net_sock_timeout_recv(fd, rdata, rlen, utimeout);
	if (iRet < 0){
		pperror("timeout recv error!\n");
		return -__LINE__;
	}

	return iRet;
}

int net_sock_timeout_recv(int fd, char *rdata, int *rlen, int utimeout){

	int iRet = 0;
	fd_set rset;
	int ifd_num = 0;
	struct timeval tv; 

	ifd_num = fd + 1;
	FD_ZERO(&rset);
	FD_SET(fd, &rset);
	tv.tv_sec  = utimeout/1000000;
	tv.tv_usec = utimeout%1000000;

	iRet = select(ifd_num, &rset, NULL, NULL, &tv);
	if (iRet < 0){
		pperror("select: %s\n", strerror(errno));
		return -__LINE__;
	} else if (iRet == 0){
		return 0;
	}

	iRet = recv(fd, rdata, *rlen, 0);
	if (iRet < 0){
		pperror("recv: %s\n", strerror(errno));
		return -__LINE__;
	}
	*rlen = iRet;

	return iRet;
}


int net_sock_one_send(int fd, char *data, int len)
{
	return send(fd, data, len, 0);
}

int net_sock_one_recv(int fd, char *data, int len)
{
	return recv(fd, data, len, 0);
}

int net_sock_set_utimeout(int fd, int utimeout, int type)
{
	struct timeval tv;
	tv.tv_sec = utimeout/1000000;
	tv.tv_usec = utimeout%1000000;
	return setsockopt(fd, SOL_SOCKET, type, &tv, sizeof(tv));
}

int net_sock_set_snd_utimeout(int fd, int utimeout)
{
	return net_sock_set_utimeout(fd, utimeout, SO_SNDTIMEO);
}

int net_sock_set_rcv_utimeout(int fd, int utimeout)
{
	return net_sock_set_utimeout(fd, utimeout, SO_RCVTIMEO);
}

int net_sock_get_socket_ip(int fd, const char *eth, char *ip)
{
	struct sockaddr_in *sin;
	struct ifreq ifr_ip;

	memset(&ifr_ip, 0, sizeof(struct ifreq));
	strncpy(ifr_ip.ifr_name, eth, sizeof(ifr_ip.ifr_name)-1);

	int iRet = 0;
	if ((iRet=ioctl(fd, SIOCGIFADDR, &ifr_ip)) < 0)
	{
		pperror("ioctl: %s\n", strerror(errno));
		return iRet;
	}
	sin = (struct sockaddr_in*)&ifr_ip.ifr_addr;
	strcpy(ip, inet_ntoa(sin->sin_addr));

	return 0;
}


int net_sock_get_eth_ip(const char *eth, char *ip)
{
	int fd = 0;
	if ( (fd=socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		pperror("socket: %s\n", strerror(errno));
		return fd;
	}
	if (net_sock_get_socket_ip(fd, eth, ip) != 0)
	{
		pperror("net_sock_get_socket_ip error!\n");
		return -__LINE__;
	}
	close(fd);
	return 0;
}



