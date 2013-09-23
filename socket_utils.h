#ifndef	__SOCKET_UTILS_H_albert_
#define	__SOCKET_UTILS_H_albert_	1

#include	"common_albert.h"

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef	SU_DEFAULT_LISTEN_PORT
#define	SU_DEFAULT_LISTEN_PORT	80
#endif

#ifndef	SU_DEFAULT_UDP_PORT
#define	SU_DEFAULT_UDP_PORT	8000
#endif

#define	LISTENQ	1024
#define	MAX_UDPSIZE	65535

#define	close_socket(f)	do {close((f)); (f) = -1;} while (0)

extern int reuse_addr(int sockfd);
extern int get_sendbuf_size(int sockfd, uint32_t *len);
extern int get_recvbuf_size(int sockfd, uint32_t *len);
extern int get_addr_from_device(const char *device_name, in_addr_t *addrp);
extern int set_sendbuf_size(int sockfd, uint32_t len);
extern int set_recvbuf_size(int sockfd, uint32_t len);
extern int set_recv_timeout(int sockfd, uint32_t timeout);
extern int set_recv_timeout_micro(int sockfd, uint32_t micro);
extern int set_send_timeout_micro(int sockfd, uint32_t micro);
extern int set_no_tw(int sockfd);
extern int set_nonblock_fd(int fd);
extern int set_block_fd(int fd);
extern int set_nodelay_fd(int fd);
extern int set_cork_fd(int fd);
extern int create_tcp_client_socket(void);
extern int close_tcp(int fd);
extern int connect_tcp_socket(int fd, const char *ip, const char *port);
extern int connect_tcp_socket_struct(int fd, const struct sockaddr_in *srvp);
extern int create_tcp_listen_socket(const char *ip, const char *port, int nonblock);
extern int create_tcp_listen_socket_struct(const struct sockaddr_in *srv, int nonblock);
extern int accept_tcp_socket(int listenfd, struct sockaddr_in *from, socklen_t *from_len, int nonblock);
extern int send_tcp_data(int fd, void *buf, size_t len, int no_sigpipe);
extern int send_tcp_data_n(int fd, void *buf, size_t len, int no_sigpipe);
extern int recv_tcp_data(int fd, void *buf, size_t buflen);
extern int recv_tcp_data_n(int fd, void *buf, size_t wantlen);
extern int create_udp_client_socket(void);
extern int send_udp_data(int fd, const char *ip, const char *port, const char *buf, size_t len);
extern int send_udp_data_struct(int fd, const struct sockaddr_in *des, const char *buf, size_t len);
extern int recv_udp_data(int fd, const char *ip, const char *port, char *buf, size_t len, int nonblock, int verify, struct sockaddr_in *from);
extern int recv_udp_data_struct(int fd, const struct sockaddr_in *ver, char *buf, size_t len, int nonblock, int verify, struct sockaddr_in *from);
extern int create_udp_listen_socket(const char *ip, const char *port, int nonblock);
extern int create_udp_listen_socket_struct(const struct sockaddr_in *srv, int nonblock);
extern void print_mem(unsigned char* buf, int len, int show);

#ifdef	__cplusplus
}
#endif

#endif
