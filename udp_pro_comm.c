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

#include "udp_pro_comm.h"
#include "net_comm.h"

int udp_create_sock()
{
	int fd = -1;
	fd = net_create_sock(AF_INET, SOCK_DGRAM);
	if (fd < -1)
	{
		pperror("net_create_sock error! return = %d\n", fd);
		return -__LINE__;
	}
	return fd;
}

int udp_create_nonblock_sock()
{
	int fd = -1;
	fd = udp_create_sock();
	if (fd < 0)
	{
		pperror("udp_create_sock error! return = %d\n", fd);
		return -__LINE__;
	}
	return net_sock_set_nonblock(fd);
}

int udp_sock_bind(int fd, const char* ip, int port)
{
	return net_sock_bind(fd, ip, port);
}

/* server */
int udp_create_and_bind(const char *ip,  int port)
{
	int fd = -1;
	int iRet = 0;
	fd = udp_create_sock();
	if (fd < 0){
		pperror("udp_create_sock error! return = %d\n", fd);
		return -__LINE__;
	}
	
	if ( (iRet = net_sock_bind(fd, ip, port)) < 0){
		pperror("net_sock_bind error! return = %d\n", iRet);
		return -__LINE__;
	}
	
	return fd;
}

int udp_sock_connect(int fd, const char *ip, int port){
	return net_sock_connect(fd, ip, port);
}
/* client */
int udp_create_and_connect(const char *ip, int port)
{
	int fd = -1;
	int iRet = 0;
	fd = udp_create_sock();
	if (fd < 0)
	{
		pperror("udp_create_sock error! return = %d\n", fd);
		return -__LINE__;
	}

	if ( ( iRet = net_sock_connect(fd, ip, port)) < 0){
		pperror("net_sock_connect error! return = %d\n", iRet);
		return -__LINE__;
	}
	
	return fd;	
}

int udp_sock_sendto(int fd, const char *data, int len, const char *ip,  int port)
{
	int iRet = 0;
	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(struct sockaddr_in));

	sa.sin_family = AF_INET;
	sa.sin_port   = htons(port);
	if ( inet_aton(ip, &(sa.sin_addr)) == -1){
		pperror("inet_aton error! : %s\n", strerror(errno));
		return -__LINE__;
	}
	iRet = sendto(fd, data, len, 0, (const struct sockaddr *) &sa, sizeof(struct sockaddr_in));
	if (iRet != len){
		pperror("sendto error! : %s\n", strerror(errno));
		return -__LINE__;
	}
	return iRet;	
}

int udp_sock_recvfrom(int fd, char *data, int len, int *ip, int *port)
{
	int iRet = 0;
	socklen_t socklen = 0;
	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(struct sockaddr_in));

	iRet = recvfrom(fd, data, len, 0, (struct sockaddr*)&(sa), (socklen_t *)&socklen);

	if (iRet <= 0){ // 0 shutdown
		pperror("recvfrom error! : %s\n", strerror(errno)); 
		return -__LINE__;
	}
	if (ip)   *ip   = ntohl(sa.sin_addr.s_addr);
	if (port) *port = ntohs(sa.sin_port);
	return iRet;
}

int udp_sock_sendrecv(int fd, const char *sdata, int slen, char *rdata, int *rlen, int utimeout)
{
	int iRet = 0;

	iRet = send(fd, sdata, slen, 0);
	if (iRet <=0 ){
		pperror("send error! : %s\n", strerror(errno));
		return -__LINE__;
	}
	iRet = net_sock_timeout_recv(fd, rdata, rlen, utimeout);
	if (iRet < 0){
		pperror("net_sock_timeout_recv error! return = %d\n", iRet);
		return -__LINE__;
	}
	*rlen = iRet;

	return iRet;
}


int udp_addr_sendrecv(const char *ip, int port, const char *sdata, int slen, char *rdata, int *rlen, 
		int utimeout)
{
	int iRet = 0;
	int fd = -1;
	fd = udp_create_and_connect(ip, port);
	if (fd < 0){
		pperror("udp_create_and_connect error! return = %d\n", fd);
		return -__LINE__;
	}

	iRet = udp_sock_sendrecv(fd, sdata, slen, rdata, rlen, utimeout);
	if (iRet < 0){
		pperror("udp_sock_sendrecv error! return = %d\n", iRet);
		return -__LINE__;
	}
	return iRet;
}


