#include "UDPIPInterface.h"

char *set_up_recv_sock(int *sock,char *addr, uint16_t port, char *received_packet){
	struct sockaddr_in receivedfrom_addr;
	recv_sockaddr(sock,addr, port, &receivedfrom_addr);
	return sock_recv(sock, &receivedfrom_addr, received_packet);
}

int UDPtoIP(char *packet) {
	return 0;
}

 

int fragment_large_packets() {
	return 0;
}

 

int assemble_broken_fragments() {
	return 0;
}

 

int send_in_order() {
	return 0;
}
