#ifndef __NET_COMM_H__
#define __NET_COMM_H__

#ifdef __DEBUG__
#define pperror(fmt, args...) \
	do{ \
		printf(fmt, ##args); \
	}while(0)
#else
#define pperror(fmt, args...) 
#endif 


#ifdef __cplusplus
extern "C" {
#endif

int net_create_sock(int domain, int type);

int net_sock_set_nonblock( int fd);

int net_sock_connect(int fd, const char *ip, int port);

int net_sock_bind(int fd, const char *ip, int port) ;

int net_sock_get_buffer(int fd, long *buflen, int type);
int net_sock_get_sndbuffer(int fd, long *buflen);
int net_sock_get_rcvbuffer(int fd, long *buflen);

int net_set_sock_utimeout(int fd, int utimeout, int type);
int net_set_sock_rcv_utimeout(int fd, int utimeout);
int net_set_sock_snd_utimeout(int fd, int utimeout);


int net_sock_set_buffer( int fd, long buflen, int type);
int net_sock_set_sndbuffer( int fd, long buflen);
int net_sock_set_rcvbuffer( int fd, long buflen);

int net_sock_one_send(int fd, char *data, int len);
int net_sock_one_recv(int fd, char *data, int len);
int net_sock_timeout_recv(int fd, char *rdata, int *rlen, int utimeout);
int net_sock_sendrecv(int fd, char *sdata, int slen, char *rdata, int *rlen, 
		int utimeout);

int net_sock_get_eth_ip(const char *eth, char *ip);
int net_sock_get_socket_ip(int fd, const char* eth, char *ip);

#ifdef __cplusplus
}
#endif


#endif 
