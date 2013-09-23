



TARGET= tcp_echo_server tcp_echo_client udp_echo_server udp_echo_client 
INC_ALL=
LIB_ALL=
#/////////////////////////////////////////////
CC=gcc
CXX=g++
CFLAGS= -Wall -g -D__DEBUG__


all:$(TARGET)

.SUFFIXES:.cpp .o


.c.o:
	$(CC) -c $^ $(CFLAGS) $(INC_ALL)
.cpp.o:
	$(CXX) -c $^ $(CFLAGS) $(INC_ALL)
.o:
	$(CXX) -o $@ $(CFLAGS) $^ $(LIB_ALL)

tcp_echo_server: tcp_echo_server.o net_comm.o tcp_pro_comm.o 
tcp_echo_client: tcp_echo_client.o net_comm.o tcp_pro_comm.o 

udp_echo_server: udp_echo_server.o net_comm.o udp_pro_comm.o 
udp_echo_client: udp_echo_client.o net_comm.o udp_pro_comm.o 

clean:
	rm -rf *.o $(TARGET)
