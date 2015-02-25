#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int server(Str address, uint16_t port)
{
	int sock, accepted_client, client_addr_len;
	struct sockaddr_in socket_addr, client_addr;
	pid_t pid;
	char packet[//some lwngth]

  /* Create UDP socket*/
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("UDP socket creation error:");
		return 1;
	}

	printf("Socket created\n");

	socket_addr.sin_addr.s_addr = inet_addr(address);
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(port);
	
	/*Bind socket to an address*/
	if (bind(sock, (struct sockaddr *) &socket_addr, sizeof(socket_addr)) < 0) {
		perror("Bind error");
		return 1;
	}
	
	
	printf("Listening for connections...\n");
	/*Listen for connections */
	listen(sock, 0);
	
	/* Accept only one connection*/	
  accepted_client = accept4(sock, (struct sockaddr *) &client_addr, &client_addr_len,0);
	printf("Accepted client: %i \n", accepted_client);
	if (accepted_client < 0) {
      		perror("Accept error");
      		return 1;
  }
	while(1){ 
	
		if(recv(accepted_client, packet, //some packet length, 0) < 0) {
			perror("Recv error:");
			return 1;
		}

		memset(reply,0,sizeof(reply));
		reply[strlen(reply)] = '\0';
		if (send(//something) < 0) {
			perror("Send error:");
			return 1;
		}
	}
	return 0;
}
