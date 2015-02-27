int create_socket(int *sock);
int connect_to_server(int *sock, const char *addr, uint16_t port);
int bind_node_addr(int *sock, const char *addr, uint16_t port);
