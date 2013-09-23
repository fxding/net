#ifndef	G_LOG_DOMAIN
#define	G_LOG_DOMAIN	"socket_utils"
#endif

#include	"socket_utils.h"
#include	"program_utils.h"
#include	"albert_log.h"

extern uint32_t g_log_level;
extern CRollLog *g_albert_logp;			/* extern log */

int
reuse_addr(int sockfd)
{
	const int on = 1;

	return setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

int
get_sendbuf_size(int sockfd, uint32_t *len)
{
	socklen_t the_len = sizeof(*len);
	return getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, len, &the_len);
}

int
get_recvbuf_size(int sockfd, uint32_t *len)
{
	socklen_t the_len = sizeof(*len);
	return getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, len, &the_len);
}

int
get_addr_from_device(const char *device_name, in_addr_t *addrp)
{
	int _sock_fd;
	struct ifconf conf;
	struct ifreq *ifr;		
	char buff[1024];	
	int num;
	int i;

	if (!addrp)
		return -1;
	
	_sock_fd = socket(PF_INET, SOCK_DGRAM, 0);
	if ( _sock_fd < 0 )	return -1;
		
	conf.ifc_len = sizeof(buff);
	conf.ifc_buf = buff;
		
	if ( ioctl(_sock_fd, SIOCGIFCONF, &conf) < 0 )
	{
		close(_sock_fd);
		return -1;
	}
		
	num = conf.ifc_len / sizeof(struct ifreq);
	ifr = conf.ifc_req;
		
	for( i=0; i<num;i++ )
	{
		struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr->ifr_addr);
			
		if ( ioctl(_sock_fd, SIOCGIFFLAGS, ifr) < 0 )
		{
			close(_sock_fd);
			return -1;
		}
		
		if ( (ifr->ifr_flags & IFF_UP) && strcmp(device_name, ifr->ifr_name) == 0 )
		{
			memcpy(addrp, &(sin->sin_addr.s_addr), sizeof(in_addr_t));
			close(_sock_fd);
				
			return 0;
		}
		ifr++;
	}
	close(_sock_fd);
		
	return -1;
}

int
set_sendbuf_size(int sockfd, uint32_t len)
{
	return setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &len, sizeof(len));
}

int
set_recvbuf_size(int sockfd, uint32_t len)
{
	return setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &len, sizeof(len));
}

int
set_recv_timeout(int sockfd, uint32_t timeout)
{
	struct timeval tv;

	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	
	return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

int
set_recv_timeout_micro(int sockfd, uint32_t micro)
{
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = micro;
	
	return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

int
set_send_timeout_micro(int sockfd, uint32_t micro)
{
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = micro;
	
	return setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

int
set_no_tw(int sockfd)
{
	struct linger lingers;

	lingers.l_onoff =1;
	lingers.l_linger =0;
	return setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &lingers, sizeof(lingers));
}

int
set_nonblock_fd(int fd)
{
	int opts;
	opts=fcntl(fd, F_GETFL);
	if(opts<0)
	{
		g_warn("fcntl(sock,GETFL) failed: %s", strerror(errno));
		return -1;
	}
	
	opts = opts|O_NONBLOCK;
	if(fcntl(fd, F_SETFL,opts)<0)
	{
		g_warn("fcntl(sock,SETFL,opts) failed: %s", strerror(errno));
		return -2;
	}   

	return 0;
}

int
set_block_fd(int fd)
{
	int opts;
	opts=fcntl(fd, F_GETFL);
	if(opts<0)
	{
		g_warn("fcntl(sock,GETFL) failed: %s", strerror(errno));
		return -1;
	}
	
	opts = opts & ~O_NONBLOCK;
	if(fcntl(fd, F_SETFL,opts)<0)
	{
		g_warn("fcntl(sock,SETFL,opts) failed: %s", strerror(errno));
		return -2;
	}   
	
	return 0;
}

int
set_nodelay_fd(int fd)
{
	int flag;

	/* Disable the Nagle (TCP No Delay) algorithm */
	flag = 1;
	return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
}

int
set_cork_fd(int fd)
{
	const int on = 1;

	return setsockopt(fd, IPPROTO_TCP, TCP_CORK, &on, sizeof(on));
}

int
clear_cork_fd(int fd)
{
	const int off = 0;

	return setsockopt(fd, IPPROTO_TCP, TCP_CORK, &off, sizeof(off));
}

int
create_tcp_client_socket(void)
{
	int sockfd;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		g_warn("socket() failed: %s", strerror(errno));
		return -1;
	}
	
	return sockfd;
}

int
close_tcp(int fd)
{
	shutdown(fd, SHUT_RDWR);
	return close(fd);
}

inline static int
_connect_tcp(int fd, const struct sockaddr_in *srvp)
{
	return connect(fd, (struct sockaddr *)srvp, sizeof(struct sockaddr_in));
}

int
connect_tcp_socket(int fd, const char * ip, const char * port)
{
	struct sockaddr_in srv;

	if (!ip || !port) {
		g_warn("invalid argument, ip: %p, port: %p", ip, port);
		return -1;
	}
	
	memset(&srv, 0, sizeof(srv));
	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = inet_addr(ip);
	srv.sin_port = htons(atoi(port));
	
	return _connect_tcp(fd, &srv);
}

int
connect_tcp_socket_struct(int fd, const struct sockaddr_in * srvp)
{
	return _connect_tcp(fd, srvp);
}

static int
_create_tcp_listen(const struct sockaddr_in *srvp, int nonblock)
{
	int fd, res;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		g_warn("socket() failed: %s", strerror(errno));
		return -1;
	}

	reuse_addr(fd);

	if (nonblock)
		set_nonblock_fd(fd);
	
	res = bind(fd, (struct sockaddr *)srvp, sizeof(*srvp));
	if (res < 0) {
		g_warn("bind() failed: %s, ip: %s, port: %u", strerror(errno), inet_ntoa(*((struct in_addr *)(&srvp->sin_addr.s_addr))), ntohs(srvp->sin_port));
		close(fd);
		return -2;
	}

	res = listen(fd, LISTENQ);
	if (res < 0) {
		g_warn("listen() failed: %s", strerror(errno));
		close(fd);
		return -3;
	}
	
	return fd;
}

int
create_tcp_listen_socket(const char * ip, const char * port, int nonblock)
{
	struct sockaddr_in srvaddr;

	memset(&srvaddr, 0, sizeof(srvaddr));
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvaddr.sin_port = htons(SU_DEFAULT_LISTEN_PORT);
	
	if (ip) {
		srvaddr.sin_addr.s_addr = inet_addr(ip);
	}

	if (port) {
		srvaddr.sin_port = htons(atoi(port));
	}

	return _create_tcp_listen(&srvaddr, nonblock);
}

int
create_tcp_listen_socket_struct(const struct sockaddr_in *srvp, int nonblock)
{
	struct sockaddr_in srvaddr;

	memset(&srvaddr, 0, sizeof(srvaddr));
	
	if (srvp)
		memcpy(&srvaddr, srvp, sizeof(struct sockaddr_in));
	else {
		srvaddr.sin_family = AF_INET;
		srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		srvaddr.sin_port = htons(SU_DEFAULT_LISTEN_PORT);
	}

	return _create_tcp_listen(&srvaddr, nonblock);
}

int
accept_tcp_socket(int listenfd, struct sockaddr_in *from, socklen_t *from_len, int nonblock)
{
	int connfd;

	connfd = accept(listenfd, (struct sockaddr *)from, from_len);
	if (connfd < 0) {
		g_warn("accept() failed: %s", strerror(errno));
		return connfd;
	}

	if (nonblock)
		set_nonblock_fd(connfd);
	
	return connfd;
}

int
send_tcp_data(int fd, void *buf, size_t len, int no_sigpipe)
{
	if (no_sigpipe)
		return send(fd, buf, len, MSG_NOSIGNAL);
	else
		return send(fd, buf, len, 0);
}

int
send_tcp_data_n(int fd, void *buf, size_t len, int no_sigpipe)
{
	if (no_sigpipe)
		return sendn(fd, buf, len, MSG_NOSIGNAL);
	else
		return sendn(fd, buf, len, 0);
}

int
recv_tcp_data(int fd, void *buf, size_t buflen)
{
	return read(fd, buf, buflen);
}

int
recv_tcp_data_n(int fd, void *buf, size_t wantlen)
{
	return readn(fd, (char *)buf, wantlen);
}

int
create_udp_client_socket(void)
{
	int sockfd;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		g_warn("socket() failed: %s", strerror(errno));
		return -1;
	}
	
	return sockfd;
}

static int
_sendto_udp(int fd, const struct sockaddr_in *des, const char * buf, size_t len)
{
	int res;
	
	res = sendto(fd, buf, len, 0, (struct sockaddr *)des, sizeof(*des));
	if (res < 0) {
		g_warn("sendto() failed: %s", strerror(errno));
		return res;
	}

	return res;
}

int
send_udp_data(int fd, const char *ip, const char *port, const char *buf, size_t len)
{
	struct sockaddr_in srv;

	if (!ip || !port || !buf) {
		g_warn("invalid argument, ip: %p, port: %p, buf: %p", ip, port, buf);
		return -10;
	}
	
	memset(&srv, 0, sizeof(srv));
	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = inet_addr(ip);
	srv.sin_port = htons(atoi(port));

	return _sendto_udp(fd, &srv, buf, len);
}

int
send_udp_data_struct(int fd, const struct sockaddr_in *des, const char * buf, size_t len)
{
	return _sendto_udp(fd, des, buf, len);
}

static int
_recvfrom_udp(int fd, char *buf, size_t len, int nonblock, const struct sockaddr_in *verify, struct sockaddr_in *from)
{
	int res;
	socklen_t fromlen;

	if (!from) {
		g_warn("from is NULL: %p", from);
		return -10;
	}
	
RE_RECV:
	memset(from, 0, sizeof(*from));
	fromlen = sizeof(*from);
	if (nonblock) {		/* nonblock */
		res = recvfrom(fd, buf, len, MSG_DONTWAIT, (struct sockaddr *)from, &fromlen);
	}
	else {
		res = recvfrom(fd, buf, len, 0, (struct sockaddr *)from, &fromlen);
	}

	if (res < 0) {
		g_warn("recvfrom() failed: %s", strerror(errno));
		return res;
	}
	
	if (verify) {		/* verify address */
		if (verify->sin_addr.s_addr != from->sin_addr.s_addr || verify->sin_port != from->sin_port)
			goto RE_RECV;
	}
	
	return res;
}

int
recv_udp_data(int fd, const char *ip, const char *port, char *buf, size_t len, int nonblock, int verify, struct sockaddr_in *from)
{
	struct sockaddr_in srv;

	if (ip && port && verify) {
		memset(&srv, 0, sizeof(srv));
		srv.sin_family = AF_INET;
		srv.sin_addr.s_addr = inet_addr(ip);
		srv.sin_port = htons(atoi(port));

		return _recvfrom_udp(fd, buf, len, nonblock, &srv, from);
	}
	else
		return _recvfrom_udp(fd, buf, len, nonblock, 0, from);
}

int
recv_udp_data_struct(int fd, const struct sockaddr_in *ver, char * buf, size_t len, int nonblock, int verify, struct sockaddr_in * from)
{
	if (ver && verify)
		return _recvfrom_udp(fd, buf, len, nonblock, ver, from);
	else 
		return _recvfrom_udp(fd, buf, len, nonblock, 0, from);
}

static int
_create_udp_listen(const struct sockaddr_in * srvp, int nonblock)
{
	int fd, res;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		g_warn("socket() failed: %s", strerror(errno));
		return -1;
	}

	reuse_addr(fd);

	if (nonblock)
		set_nonblock_fd(fd);
	
	res = bind(fd, (struct sockaddr *)srvp, sizeof(*srvp));
	if (res < 0) {
		g_warn("bind() failed: %s", strerror(errno));
		close(fd);
		return -2;
	}
	
	return fd;
}

int
create_udp_listen_socket(const char *ip, const char *port, int nonblock)
{
	struct sockaddr_in srv;	

	memset(&srv, 0, sizeof(srv));
	srv.sin_family = AF_INET;
	
	if (ip)
		srv.sin_addr.s_addr = inet_addr(ip);
	else
		srv.sin_addr.s_addr = htonl(INADDR_ANY);

	if (port)
		srv.sin_port = htons(atoi(port));
	else
		srv.sin_port = htons(SU_DEFAULT_UDP_PORT);

	return _create_udp_listen(&srv, nonblock);
}

int
create_udp_listen_socket_struct(const struct sockaddr_in * srvp, int nonblock)
{
	struct sockaddr_in srvaddr;

	memset(&srvaddr, 0, sizeof(srvaddr));
	
	if (srvp)
		memcpy(&srvaddr, srvp, sizeof(struct sockaddr_in));
	else {
		srvaddr.sin_family = AF_INET;
		srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		srvaddr.sin_port = htons(SU_DEFAULT_UDP_PORT);
	}

	return _create_udp_listen(&srvaddr, nonblock);
}

void
print_mem(unsigned char* buf, int len, int show)
{
	int i = 0;
	for(i = 0; i < len; i++)
	{
		char c = buf[i];

		char szResult[3] = {0};
		int   i1 = (c & 0xF0) >> 4 ;
		int   i2 = c & 0x0F;
		snprintf(szResult, 3, "%X%X", i1, i2);

	  if(i % 10 == 0 && i > 0)
	  {
		  printf("%s \r\n", szResult);
	  }
	  else
	  {
		  printf("%s ", szResult);
	  }
	}

	if(show)
	{
		for(i = 0; i < len; i++)
		{
			printf("%c", buf[i]);
		}
	}
}


