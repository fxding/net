
#include "net_comm.h"
#include "tcp_pro_comm.h"
#include "udp_pro_comm.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]){

	int fd = -1;

	if (argc < 4){
		printf("./exe ip, port string\n");
		return 1;
	}

	fd = udp_create_sock();
/*
	udp_sock_connect(fd, argv[1], atoi(argv[2]));
*/
//	fd = udp_create_and_connect(argv[1], atoi(argv[2]));

	int len = 0; 
	printf("init done....\n");
	while(1){	
		//len = net_sock_one_send(fd, argv[3], strlen(argv[3]));
		len = udp_sock_sendto(fd, argv[3], strlen(argv[3]), argv[1], atoi(argv[2]));
		if (len < 0){
			printf("error:%s\n", strerror(errno));
		}
		printf("send:%d-%s\n", len, argv[3]);
		sleep(2);
	}
	return 0;
}
