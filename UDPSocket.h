int create_socket(int *sock);
int connect_to_server(int *sock, const char *addr, uint16_t port);
int bind_node_addr(int *sock, const char *addr, uint16_t port);
int sock_listen(int *sock, int backlog);
int node_accept_and_process(int *accepted_client_sock, int *sock);
