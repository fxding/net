
#include "net_comm.h"
#include "tcp_pro_comm.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
*/

#include <string.h>

int main(int argc, char *argv[]){

	int fd = -1;
	if (argc < 4){
		printf("./exe ip port string\n");
		return 0;
	}

	int iRet = 0;
	/*
	fd = tcp_create_sock();
	if (fd < 0){
		printf("tcp_create_sock error! fd=%d\n", fd);
		return 0;
	}

	iRet = tcp_sock_connect(fd, argv[1], atoi(argv[2]));
	if (iRet < 0){
		printf("tcp_sock_connect error! iRet=%d\n", iRet);
		return 1;
	}
	*/
	fd = tcp_create_and_connect(argv[1], atoi(argv[2]));
	if (fd < 0){
		printf("tcp_create_and_connect error! fd=%d\n", fd);
		return 0;
	}
	char data[256];
	int len = sizeof(data);

	printf("waiting.....\n");
	while(1){
		len = net_sock_one_send(fd, argv[3], strlen(argv[3]));
		if (len <= 0){
			printf("net_sock_one_send error! len=%d\n", len);
			break;
		}
		printf("send=%d-%s\n", len, argv[3]);

		len = sizeof(data);
		len = tcp_sock_timeout_recv(fd, data, &len, 10000101);
		//len = net_sock_one_recv(cltfd, data, len);
		if (len < 0){
			printf("tcp_sock_timeout_recv error! len=%d\n", len);
			break;
		}
		data[len] = '\0';
		printf("recv:%d-%s\n", len, data);
		sleep(2);
	}
	return 0;
}
