
#include "net_comm.h"
#include "tcp_pro_comm.h"


#include <stdio.h>
#include <stdlib.h>
/*
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
*/

#include <string.h>

int main(int argc, char *argv[]){

	int fd = -1;
	if (argc < 3){
		printf("./exe ip port\n");
		return 0;
	}
	/*
	   fd = tcp_create_sock();

	   tcp_sock_bind(fd, 0, 201349);

	   tcp_sock_listen(fd, 10);
	   */
	fd = tcp_create_and_listen(argv[1], atoi(argv[2]), 10);
	if (fd < 0){
		printf("tcp_create_and_listen error! fd=%d\n", fd);
		return 1;
	}
	printf("waiting.....\n");
	int cltfd = tcp_sock_accept(fd, NULL);
	if (cltfd < 0){
		printf("tcp_sock_accept error! fd=%d\n", cltfd);
		return 2;
	}

	char data[256];
	int len = sizeof(data);

	while(1){
		len = sizeof(data);
		//len = tcp_sock_timeout_recv(cltfd, data, &len, 10000101);
		len = net_sock_one_recv(cltfd, data, len);
		if (len <=0){
			printf("net_sock_one_recv error! len=%d\n", len);
			break;
		}
		data[len] = '\0';
		printf("recv:%d-%s\n", len, data);
		len = net_sock_one_send(cltfd, data, len);
		if (len <= 0){
			printf("net_sock_one_send error! len=%d\n", len);
			break;
		}
		printf("send=%d-%s\n", len, data);
	}
	return 0;
}
