#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int create_socket(int *sock){
	if ((*sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Create socket error:");
		return 1;
	}
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


	sendaddr.sin_addr.s_addr = inet_addr(addr);
	sendaddr.sin_family = AF_INET;
	sendaddr.sin_port = htons(port);

	if (sendto(*sock, packet, 1400/*Enforce MTU */, 0, (struct sockaddr *)&sendaddr, sizeof(sendaddr)) < 0) {
		perror("sendto failed");
		return 1;
	}
	return 0;
}

int sock_recv(int *sock,char *addr, uint16_t port, char *received_packet){
	struct sockaddr_in receivedfrom_addr;
	int recvlen;

	receivedfrom_addr.sin_addr.s_addr = inet_addr(addr);
	receivedfrom_addr.sin_family = AF_INET;
	receivedfrom_addr.sin_port = htons(port);

	while(1) { /* Listens on one interface */
                recvlen = recvfrom(*sock, received_packet, 64000, 0, (struct sockaddr *)&receivedfrom_addr, NULL);
                printf("received %d bytes\n", recvlen);
                if (recvlen > 0) {
                        received_packet[recvlen] = 0;
                        printf("received message: \"%s\"\n", received_packet);
                }
        }
}


