#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>
#include "UDPIPInterface.h"
#include "IPRIPInterface.c"

int parse_file(char *);
int create_listening_sock();
int ifconfig();
int changeUpDown();
void *receive_func(void *);

#define MAXENTRY 1024
#define INFINITY 16

struct node_interface {
	int id;
	int port;
	char ipAddr[32];
	char vipThis[32];
	char vipRemote[32];
	char status[32]; //up by default
};

struct thread_arg_list{
	int *sock;
	char *addr;
	uint16_t port;
	char *received_packet;
};

struct node_interface interfaces[1024];
pthread_t children_tid[1024];
int count;
int *sock;
char ipAddrThis[32];

int main(int argc, char* argv[]) {
	if (argc > 2) {
		printf("Incorrect input\n");
		return 1;
	}
	parse_file(argv[1]);
	create_listening_sock();
	test_send();
	struct node_interface *interface_pointer = interfaces;

	while (interface_pointer->id != 0) {
		//update fwding table
		update_fwd_table(interface_pointer->ipAddr, interface_pointer->ipAddr, INFINITY);
		//send RIP request
		send_RIP_request(interface_pointer->id, ipAddrThis, interface_pointer->ipAddr);
	}	
	
	while (1) {};

	close(*sock);

	return 0;
}

int parse_file(char *path){
	struct node_interface *copy = interfaces;
	char line[256];
	char *split, *colon, *ipAddr, *vipThis, *vipRemote;
	int port;
	pid_t pid;

	FILE *fr = fopen(path, "r");
	count = 0;
	while (fgets(line, sizeof(line), fr)) {
		printf("\n%s", line);
		split = strtok(line, ":"); //segment before ":"

		//set up ipAddr variable
		if (strcmp(split, "localhost") == 0) {
			ipAddr = "127.0.0.1";
		} else {
			ipAddr = split;
		}
		printf("IPADDR: %s\n", ipAddr);

		split = strtok(NULL, ":"); //segment after ":"
		
		if (count == 0) { //first line; "port" is this node's port
			strcpy(ipAddrThis, ipAddr);
			port = (uint16_t) atoi(split);
			printf("PORT: %d\n", port);

			int on = 1;
			sock = (int *)malloc(sizeof(int));
			create_socket(sock);		
			bind_node_addr(sock, ipAddr, (uint16_t) port);

		} else { //not first line; "port" is remote host's port, and 
			 //there should be VIPs following
			split = strtok(split, " "); 
			port = atoi(split);
			printf("PORT: %d\n", port);
			split = strtok(NULL, " "); //VIP of this node's interface
			if (split == NULL) {
				printf("Incorrect file format\n");
				return 1;
			}
			vipThis = split;

			split = strtok(NULL, " "); //VIP of remote node's interface
			if (split == NULL) {
				printf("Incorrect file format\n");
				return 1;
			}
			vipRemote = split;
			
			//create interface object
			copy->id = count;
			copy->port = port;
			strcpy(copy->ipAddr, ipAddr);
			strcpy(copy->vipThis, vipThis);
			strcpy(copy->vipRemote, vipRemote);
			strcpy(copy->status, "up");
			copy++;

			//test:
			ifconfig();
			changeUpDown("down", 1);
			changeUpDown("down", 2); //should fail in 1st run of else branch
		}
		count++;
	}
	fclose(fr);
	return 0;
}

int create_listening_sock(){
	int i = 0;
	while(interfaces[i].id != 0 && i < MAXENTRY){
		char *received_packet;
		received_packet = (char *)malloc(64000);
		
		
		struct thread_arg_list *arg_list;
		arg_list = (struct thread_arg_list*)malloc(sizeof(struct thread_arg_list));
		arg_list->sock = sock;
		arg_list->addr = interfaces[i].ipAddr;
		arg_list->port = interfaces[i].port;
		arg_list->received_packet = received_packet;
		//create thread
		int result;		
		if((result = pthread_create(&children_tid[i], NULL, receive_func,arg_list))){
		perror("Create thread Failed");
		return 1;
		}	
			
		i++;
	}
	return 0;
}

int ifconfig() {
	int i = 0;
	for(i = 0; i < count; i++) {
		printf("%d\t%s\t%s\n", interfaces[i].id, interfaces[i].vipThis, interfaces[i].status);
		//for testing:
		printf(" ipAddr: %s\tport: %d\n", interfaces[i].ipAddr, interfaces[i].port);
	}
	return 0;
}

int changeUpDown (char *upOrDown, int id) {
	int i = 0;
	for(i = 0; i < count; i++) {
		if(id == interfaces[i].id) {
			strcpy(interfaces[i].status, upOrDown);
			printf("Interface %d %s\n", interfaces[i].id, interfaces[i].status);
			//triggered updates here
			return 0;
		}
	}
	printf("Interface %d not found\n", id);
	return 1;
}

/* for now its just printing but eventually all receives must be dealt with here */
void *receive_func(void *arg) {
	struct thread_arg_list *args = (struct thread_arg_list *)arg;

  	set_up_recv_sock(args->sock,args->addr, args->port, args->received_packet);
 	
  	pthread_exit(NULL);
}


int send_RIP_request(int id, char ipAddrSrc, char ipAddrDst) {
	struct entry *entries[1024];
	int entries = 0; 
	memset(entries, 0, sizeof(entries));
	struct ip_packet *ipPacket = construct_IP_packet(num_entries, entries, 1, id, ipAddrSrc, ipAddrDst, INFINITY);

	//send it here

	return 0;
}

int send_RIP_response(int id, char ipAddrSource, char ipAddrDest) {
	struct entry *entries[1024];
	int num_entries = 0;
	
	struct entry *entry_pointer = entries;
	struct node_interface *interface_pointer = interfaces;
	struct fwd_entry *fwd_pointer;
	while(interface_pointer->id != 0) {
		fwd_pointer = fwd_table;
		while (fwd_pointer->destIPAddr != interface_pointer->ipAddr) {
			void *pointer;
			pointer += sizeof(struct fwd_entry);
			fwd_pointer = (fwd_entry *)pointer;
		}
		if (fwd_pointer->destIPAddr <= 0) {
			printf("Error with finding correct destination IP address in send_RIP_response\n");
			return 1;
		}
		entry_pointer->cost = (uint32_t) fwd_pointer->cost;
		entry_pointer->address = (uint32_t) inet_addr(fwd_pointer->destIPAddr);
		void *epointer;
		epointer += sizeof(struct entry);
		entry_pointer = (struct entry) epointer;
		num_entries++;

		void *ipointer;
		ipointer += sizeof(struct node_interface);
		interface_pointer = (struct node_interface *)ipointer;
	}
	
	struct ip_packet *ipPacket = construct_IP_packet(num_entries, entries, 2, id, ipAddrSrc, ipAddrDst, INFINITY);

	//send it here

	return 0;
}

int routes() {
	//interact with interface to find and print all next-hops; consult table
}

int send_message(char *vip, char *message) {
	//find appropriate next hop (consult table) and send
}

int test_send(){
	int i = 0;
	while(i != 10){
		char packet[512] = "baka baka baka!!\n";
		sock_send(sock, "127.0.0.1", 17001, packet);
		i++;
	}
}

int receive_message() {

}
