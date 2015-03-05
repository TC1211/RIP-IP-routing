#include "UDPSocket.h"

int create_socket(int *sock){

	if ((*sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Create socket error:");
		return 1;
	}
	int on=1;
	setsockopt(*sock, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	printf("Socket created, Handle: %i\n", *sock);
	return 0;
}

int bind_node_addr(int *sock, const char *addr, uint16_t port){
	struct sockaddr_in socket_addr;

	socket_addr.sin_addr.s_addr = inet_addr(addr);
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(port);
	
	/*Bind socket to an address*/
	if (bind(*sock, (struct sockaddr *) &socket_addr, sizeof(socket_addr)) < 0) {
		perror("Bind error");
		return 1;
	}
	printf("Bound to Node VIP %s, port number: %d\n", addr, port);
	return 0;
}

int sock_send(int *sock, char *addr, uint16_t port, char* packet){
	struct sockaddr_in sendaddr;
	ssize_t length;

	sendaddr.sin_addr.s_addr = inet_addr(addr);
	sendaddr.sin_family = AF_INET;
	sendaddr.sin_port = htons(port);
	//printf("%s", packet);
	if ((length = sendto(*sock, packet, 1400/*Enforce MTU */, 0, (struct sockaddr *)&sendaddr, sizeof(sendaddr))) < 0) {
		perror("sendto failed");
		return 1;
	}
	return 0;
}

int recv_sockaddr(int *sock,char *addr, uint16_t port, struct sockaddr_in *receivedfrom_addr){

	receivedfrom_addr->sin_addr.s_addr = inet_addr(addr);
	receivedfrom_addr->sin_family = AF_INET;
	receivedfrom_addr->sin_port = htons(port);

	printf("listening to port: %i\n", (int)port);
	return 0;
}

char *sock_recv(int *sock,struct sockaddr_in *receivedfrom_addr, char *received_packet){
	int recvlen = 0;
	socklen_t fromlen;
	
	fromlen = sizeof(receivedfrom_addr);
	printf("got here\n");
        recvlen = recvfrom(*sock, received_packet, 64000, 0, (struct sockaddr *)&receivedfrom_addr, &fromlen);
        if (recvlen > 0) {
            	received_packet[recvlen] = 0;
        }
	return received_packet;
}
