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

int connect_to_server(int *sock, const char *addr, uint16_t port){
	struct sockaddr_in server_addr;

	server_addr.sin_addr.s_addr = inet_addr(addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	
	if (connect(*sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		perror("Connect error:");
		return 1;
	}
	printf("Connected to server %s:%d\n", addr, port);
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
