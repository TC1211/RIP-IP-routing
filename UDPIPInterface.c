#include "IP.c"
#include "UDPSocket.h"

int set_up_recv_sock(int *sock,char *addr, uint16_t port, char *received_packet){
	struct sockaddr_in receivedfrom_addr;
	recv_sockaddr(sock,addr, port, &receivedfrom_addr);
	sock_recv(sock, &receivedfrom_addr, received_packet);
	return 0;
}

int UDPtoIP(char *packet){

}

 

int fragment_large_packets(){

}

 

int assemble_broken_fragments(){

}

 

int send_in_order(){

}
