#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int create_socket(int *sock);
int bind_node_addr(int *sock, const char *addr, uint16_t port);
int sock_send(int *sock, char *addr, uint16_t port, char* packet);
int recv_sockaddr(int *sock,char *addr, uint16_t port, struct sockaddr_in *receivedfrom_addr);
char *sock_recv(int *sock,struct sockaddr_in *receivedfrom_addr, char *received_packet);

#endif
