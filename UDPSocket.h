#include <stdint.h>

int create_socket(int *sock);
int bind_node_addr(int *sock, const char *addr, uint16_t port);
int sock_send(int *sock, char *addr, uint16_t port, char* packet);
int recv_sockaddr(int *sock,char *addr, uint16_t port, struct sockaddr_in *receivedfrom_addr);
int sock_recv(int *sock,struct sockaddr_in *receivedfrom_addr, char *received_packet);
