int create_socket(int *sock);
int bind_node_addr(int *sock, const char *addr, uint16_t port);
int sock_send(int *sock, char *addr, uint16_t port, char* packet);
int sock_recv(int *sock,char *addr, uint16_t port, char *received_packet);
