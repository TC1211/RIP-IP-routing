#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>
#include "UDPIPInterface.h"
#include "UDPSocket.h"
#include "IPRIPInterface.h"

int parse_file(char *);
int create_listening_sock();
int ifconfig();
int changeUpDown();
void *receive_func(void *);

extern int create_socket(int *sock);
extern int bind_node_addr(int *sock, const char *addr, uint16_t port);
extern int sock_send(int *sock, char *addr, uint16_t port, char* packet);
extern int create_fwd_table();

#define MAX_LINE 256
#define MAX_CHAR 32

typedef struct node_interface {
	int id;
	int port;
	char ipAddr[MAX_CHAR];
	char vipThis[MAX_CHAR];
	char vipRemote[MAX_CHAR];
	char status[MAX_CHAR]; //up by default
} node_interface;

struct thread_arg_list{
	int *sock;
	char *addr;
	uint16_t port;
	char *received_packet;
};

node_interface *interfaces;
pthread_t children_tid[MAX_ENTRY];
int count;
int *sock;
char ipAddrThis[MAX_CHAR];

int parse_file(char *path){
	interfaces = (node_interface *)malloc(MAX_ENTRY * sizeof(node_interface));
	node_interface *current = interfaces;
	char line[MAX_LINE];
	char *split, *ipAddr, *vipThis, *vipRemote;
	int port;

	FILE *fr = fopen(path, "r");
	count = 0;
	while (fgets(line, sizeof(line), fr) != NULL) {
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
			vipRemote = strtok(split, "\n");
			
			printf("VIPTHIS: %s\n", vipThis);
			printf("VIPREMOTE: %s\n", vipRemote);

			//create interface object
			current->id = count;
			current->port = port;
			strcpy(current->ipAddr, ipAddr);
			strcpy(current->vipThis, vipThis);
			strcpy(current->vipRemote, vipRemote);
			strcpy(current->status, "up");
			
			void *temp = (void *)current;
			temp += sizeof(node_interface);
			current = (node_interface *)temp;			
		}
		count++;
	}
	fclose(fr);
	return 0;
}

int create_listening_sock(){
	int i = 0;
	while(interfaces[i].id != 0 && i < MAX_ENTRY){
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
	for(i = 0; i < count - 1; i++) {
		printf("%d\t%s\t%s\n", interfaces[i].id, interfaces[i].vipThis, interfaces[i].status);
	}
	return 0;
}

int changeUpDown (char *upOrDown, int id) {
	int i = 0;
	for(i = 0; i < count - 1; i++) {
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


int send_RIP_request(int id, char *ipAddrSrc, char *ipAddrDst) {
	entry *ent = (entry *)malloc(sizeof(entry));
	int num_entries = 0; 
	memset(ent, 0, sizeof(ent));
	ip_packet *ipPacket = construct_RIP_packet_intf(num_entries, ent, 1, id, ipAddrSrc, ipAddrDst, INFINITY); 

	//send ipPacket here

	free(ent);
	return 0;
}

int send_RIP_response(int id, char *ipAddrSource, char *ipAddrDest) {
	entry *entries = (entry *)malloc(sizeof(entry));
	int num_entries = 0;
	
	entry *entry_pointer = entries;
	node_interface *interface_pointer = interfaces;
	fwd_entry *fwd_pointer;
	while(interface_pointer->ipAddr != '\0') {
		fwd_pointer = fwd_table;
		while (strcmp(fwd_pointer->destVIPAddr, interface_pointer->vipRemote) != 0) {
			void *pointer;
			pointer += sizeof(fwd_entry);
			fwd_pointer = (fwd_entry *)pointer;
		}
		if (fwd_pointer->destVIPAddr <= 0) {
			printf("Error with finding correct destination IP address in send_RIP_response\n");
			return 1;
		}
		entry_pointer->cost = (uint32_t) fwd_pointer->cost;
		entry_pointer->address = (uint32_t) inet_addr(fwd_pointer->destVIPAddr);
		void *epointer;
		epointer += sizeof(entry);
		entry_pointer = (entry *) epointer;
		num_entries++;

		void *ipointer;
		ipointer += sizeof(node_interface);
		interface_pointer = (node_interface *)ipointer;
	}
	
	ip_packet *ipPacket = construct_RIP_packet_intf(num_entries, entries, 2, id, ipAddrSource, ipAddrDest, INFINITY); 

	//send ipPacket here

	free(entries);
	return 0;
}

int routes() {
	//interact with IPRIPInterface to find and print all next-hops; consult table
	fwd_entry *pointer = fwd_table;
	while(pointer->nextHopInterfaceID != 0) {
		printf("%s\t", pointer->destVIPAddr);
		printf("%d\t", pointer->nextHopInterfaceID);
		printf("%d\n", pointer->cost);
		
		void *temp = pointer;
		temp += sizeof(fwd_entry);
		pointer = (fwd_entry *)temp;
	}
	return 0;
}

int send_message(char *vipRemote, char *message) {
	//find appropriate next hop (consult table) and send
	//must check to see whether next hop is up or down...
	//also change to down
	printf("%s: %s\n", vipRemote, message);
	int i = 0;
	for (i = 0; i < count - 1; i++) {
		if (strcmp(interfaces[i].vipRemote, vipRemote) == 0) {
//			printf("found vipRemote %s\n", interfaces[i].vipRemote);
			ip_packet *ipPacket = construct_nonRIP_packet_intf(message, interfaces[i].id, ipAddrThis, interfaces[i].ipAddr, INFINITY); 
		}
	}
	return 0;
}

int test_send(){
	int i = 0;
	while(i != 10){
		char packet[512] = "hello world";
		sock_send(sock, "127.0.0.1", 17001, packet);
		i++;
	}
	return 0;
}

int main(int argc, char* argv[]) {
	if (argc > 2) {
		printf("Incorrect input\n");
		return 1;
	}
	parse_file(argv[1]);
	create_listening_sock();

/*	printf("**testing test_send:\n");
	test_send();
*/	create_fwd_table();
	int i = 0;
	for(i = 0; i < count - 1; i++) {
		update_fwd_table(interfaces[i].vipRemote, interfaces[i].id, INFINITY);
		send_RIP_request(interfaces[i].id, ipAddrThis, interfaces[i].ipAddr);
	}
/*	printf("\n**testing ifconfig command:\n");
	ifconfig();

	printf("\n**testing up/down commands:\n");
	printf("calling up on interface 1: ");	
	changeUpDown("up", 1);
	printf("calling up on interface 2: ");
	changeUpDown("up", 2); 
	printf("\t(should have failed for A.txt and C.txt)\n");

	printf("\n**testing routes command:\n");
	routes();
*/	
	while (1) {
		printf("\n# Enter a command: ");
		char *input = (char *)malloc(sizeof(char) * (1400 - sizeof(struct ip)));
		char *input_orig = (char *)malloc(sizeof(char) * (1400 - sizeof(struct ip)));
		fgets(input, 1400 - sizeof(struct ip), stdin);
		memcpy(input_orig, input, 1400 - sizeof(struct ip));
		
		char *newline = strchr(input, '\n');
		if (newline) {
			*newline = 0;
		}
		char *token = strtok(input, " ");
		if (token == NULL) {
			printf("Error in input format\n");
			break;
		}
		if (strcmp(token, "ifconfig") == 0) {
			ifconfig();
		} else if (strcmp(token, "routes") == 0) {
			routes();
		} else if (strcmp(token, "up") == 0 || strcmp(token, "down") == 0) {
			char *upOrDown = token;
			char *temp = strtok(NULL, " ");
			int interface = atoi(temp);
			changeUpDown(upOrDown, interface);
		} else if (strcmp(token, "send") == 0) {
			char *vipRemote = strtok(NULL, " ");
			char *message = (char *)malloc(1400 - sizeof(struct ip));
			strncpy(message, (void *)input_orig + 6 + strlen(vipRemote), 1400 - sizeof(struct ip));
			send_message(vipRemote, message);
		} 
		
	}

	close(*sock);

	return 0;
}

