#ifndef __UDP_SMP_INTERFACE__
#define __UDP_SMP_INTERFACE__




#ifdef __cplusplus
extern "C" {
#endif 


int udp_create_sock();

int udp_create_and_bind(const char *ip,  int port);

int udp_sock_bind(int fd, const char *ip, int port);

int udp_sock_connect(int fd, const char *ip, int port);

int udp_create_and_connect(const char *ip, int port);

int udp_create_nonblock_sock();

int udp_sock_sendto(int fd, const char *data, int len, const char *ip,  int port);

int udp_sock_recvfrom(int fd, char *data, int len, int *ip, int *port);

int udp_sock_sendrecv(int socket, const char *sdata, int slen, char *rdata, int *rlen, int utimeout);

int udp_addr_sendrecv(const char *ip, int port, const char *sdata, int slen, char *rdata, int *rlen, int utimeout);






#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __UDP_SMP_INTERFACE__
