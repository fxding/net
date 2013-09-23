
#ifndef __TCP_COMM_LIB__
#define __TCP_COMM_LIB__


#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Create a tcp fd 
 **/
int tcp_create_sock();
/*
 * Create a nonblock fd
 * */
int tcp_create_nonblock_sock();

int tcp_sock_connect(int fd, const char *ip, int port);
/*
 * Create and connect to ip-port
 * */
int tcp_create_and_connect(const char *ip, int port);

int tcp_sock_bind(int fd, const char *ip, int port);

int tcp_sock_accept(int fd, void * addr);

/*
 * Send data to a fd
 * */
int tcp_sock_nsend(int fd, const char *data, int len);
/*
 * Recv data from a fd
 * */
int tcp_sock_nrecv(int fd, char *data, int len);

int tcp_sock_timeout_recv(int fd, char *data, int *len, int utimeout);


/*
 * tcp socket listen
 *
 * */
int tcp_sock_listen(int fd, int backlog);

int tcp_create_and_listen(const char *ip, int port, int backlog);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__TCP_COMM_LIB__
