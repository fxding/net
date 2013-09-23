
#include "net_comm.h"
#include "tcp_pro_comm.h"
#include "udp_pro_comm.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]){

	int fd = -1;

	if (argc < 3){
		printf("./exe ip port\n");
		return 1;
	}
	/*
	fd = udp_create_sock();
	udp_sock_bind(fd, argv[1], atoi(argv[2]));
	*/

	fd = udp_create_and_bind(argv[1], atoi(argv[2]));
	if (fd < 0){
		printf("error:%s-[%d]\n", strerror(errno), fd);
		return 2;
	}
	char data[256];
	int len = sizeof(data);

	while(1){
		len = sizeof(data);
		len = net_sock_one_recv(fd, data, len);
		if (len < 0){
			printf("error:%s-[%d]\n", strerror(errno), len);
			return 3;
		}
		data[len] = '\0';
		printf("recv:%d-%s\n", len, data);
	}
	return 0;
}
