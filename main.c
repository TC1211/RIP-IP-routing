#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>
#include "UDPIPInterface.h"
#include "UDPSocket.h"
#include "IPRIPInterface.h"
 
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


int parse_file(char *);
int create_listening_sock();
int populate_fwd_table();
int initial_flood();
int ifconfig(); //ifconfig command
int changeUpDown(); //changeUpDown command
void *receive_func(void *);  
int routes(); //routes command
int send_updates(); //to be called for sending either triggered updates or periodic updates
int send_message(char *vipRemote, char *message); //create and send message (non-RIP)
int test_send();
void* next_dest(int);
node_interface* table_search(char *);
int construct_and_send_IP(int, entry *, int,  char *);
int get_src_interface_id(char *);
void *periodic_updates(void *);
 
node_interface *interfaces;
pthread_t children_tid[MAX_ENTRY];
int count;
int *sock;
char ipAddrThis[MAX_CHAR];


int main(int argc, char* argv[]) {
    	if (argc > 2) {
        	printf("Incorrect input\n");
        	return 1;
    	}
    	parse_file(argv[1]);
    	create_fwd_table();
    	populate_fwd_table();
    	create_listening_sock();
 
    	int result;
    	struct thread_arg_list *arg_list;
    	pthread_t pid;
    	if((result = pthread_create(&pid, NULL, (void *)periodic_updates,arg_list))){
        	perror("Create thread Failed");
        	return 1;
    	}   
    	sleep(1);
    	initial_flood();  
    	// This GUI command input can be also in its own thread
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
			char *msg = strtok(message, "\n");
			
			ip_packet ip[sizeof(ip_packet)];
			ip_packet *packet = ip;
			char b[sizeof(ip_packet)];
			char *buf = b;
			
			node_interface *current = (node_interface *)interfaces;
			while (strlen(current->ipAddr) != 0 && strcmp(current->vipRemote, vipRemote) != 0) {
				current++;
			}
			char *ipAddrDest = current->ipAddr;
			packet = create_IPpacket_without_RIP(msg, ipAddrThis, ipAddrDest, (uint8_t) INFINITY);
		
			IPtoUDP(packet, buf);
			printf("buf: %s\n", buf);
			send_in_order(sock, ipAddrDest, current->port, buf);
        	}  
    	}
    	close(*sock);
 	return 0;
}

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
//      printf("IPADDR: %s\n", ipAddr);
 
        split = strtok(NULL, ":"); //segment after ":"
         
        if (count == 0) { //first line; "port" is this node's port
            strcpy(ipAddrThis, ipAddr);
            port = (uint16_t) atoi(split);
//	    printf("PORT: %d\n", port);
 
            sock = (int *)malloc(sizeof(int));
            create_socket(sock);         
            bind_node_addr(sock, ipAddr, (uint16_t) port);
 
        } else { //not first line; "port" is remote host's port, and  
             //there should be VIPs following
            split = strtok(split, " ");  
            port = atoi(split);
//          printf("PORT: %d\n", port);
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
             
//          printf("VIPTHIS: %s\n", vipThis);
//          printf("VIPREMOTE: %s\n", vipRemote);
 
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
 
int populate_fwd_table() {
  node_interface *current = (node_interface *)interfaces;
  fwd_entry *pointer = fwd_table;
  while(strlen(current->ipAddr) != 0) {
    memcpy(pointer->destVIPAddr, current->vipRemote, sizeof(pointer->destVIPAddr));
    pointer->nextHopInterfaceID = current->id;
    pointer->cost = INFINITY;
    pointer->last_refresh = time(0);

    current++;
    pointer++;
  }
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
        if((result = pthread_create(&children_tid[i], NULL, (void *)receive_func,arg_list))){
        perror("Create thread Failed");
        return 1;
        }     
             
        i++;
    }
    return 0;
}
 
/* for now its just printing but eventually all receives must be dealt with here */
void *receive_func(void *arg) {
	struct thread_arg_list *args = (struct thread_arg_list *)arg;
	while(1){
	set_up_recv_sock(args->sock,args->addr, args->port, args->received_packet);
	ip_packet ip[sizeof(ip_packet)];
	ip_packet *IPpacket = ip;
	UDPtoIP(args->received_packet, IPpacket);
	if(!checksum_compute(IPpacket)) {
		return NULL; // drop packet
	}
      	struct in_addr dest_addr = IPpacket->header.ip_dst;
      	char *remoteVIP = inet_ntoa(dest_addr);
      	struct in_addr src_addr = IPpacket->header.ip_src;
      	char *srcVIP = inet_ntoa(src_addr);

      	if (strcmp(remoteVIP, interfaces->vipThis) == 0) {    
//        	if (is_RIP_packet(&IPpacket->header) == 1) {
		if (IPpacket->header.ip_p == 200) {
			printf("got an RIP packet\n");
          		rip_packet *RIPpack = (rip_packet *)malloc(sizeof(rip_packet));
          		RIPpack = deserialize_RIP(IPpacket->payload);
          		int src_id  = get_src_interface_id(srcVIP);
          		if (process_rip_command(RIPpack)){//Request
        			fwd_entry *fwd_info = fwd_table;
        			entry *RIP_entries = (entry *)malloc(sizeof(RIP_entries)*64);
        			int numEnt = 0;
        			while(fwd_info->cost !=0){
          				entry tempEntry;
          				struct in_addr dest_addr_entry;
          				inet_aton(fwd_info->destVIPAddr, &dest_addr_entry);
          				if(fwd_info->nextHopInterfaceID == src_id){   //SHRP
            					fwd_info->cost = INFINITY;
            					create_entry(&tempEntry, INFINITY, dest_addr_entry.s_addr);
          				} else {
            					create_entry(&tempEntry, fwd_info->cost, dest_addr_entry.s_addr);
          				}
          				*RIP_entries = tempEntry;
          				fwd_info++;
           				RIP_entries++;
           				numEnt++;
        			}
        			construct_and_send_IP(numEnt, RIP_entries,2, srcVIP);
          			} else { //Response->update table
        				entry *List_of_entries = RIPpack->entries;
        				int e;
        				for (e = 0 ; e < RIPpack->num_entries; e++){
          					struct in_addr tempAddr;
          					tempAddr.s_addr = List_of_entries->address;
          					char * DestAddr = inet_ntoa(tempAddr);
          					update_fwd_table(DestAddr, src_id, List_of_entries->cost + 1);
        				}
          			}
        		} else {
          			printf("# Received a message: %s", IPpacket->payload);
        	}
      	} else {
        	int id = Search_forwarding_table(remoteVIP);
        	struct node_interface *node = next_dest(id);
        	process_header_for_forwarding(&(IPpacket->header));
        	char UDPmsg[1400];
        	IPtoUDP(IPpacket, UDPmsg);
        	send_in_order(sock, node->ipAddr, node->port, UDPmsg);
      	}
}
      	pthread_exit(NULL);
}

void *periodic_updates(void *args) {
   
  while(1) {
    sleep(5);

    node_interface *current = (node_interface *)interfaces;
    int i = 0;
    //for each interface node, send everything in fwd_table
    for (i = 0; i < count - 1; i++) {
      fwd_entry *fwd = (fwd_entry *)fwd_table;
      entry list[64 * sizeof(fwd_table)];
      entry *ent = (entry *)list;
      while (strlen(fwd->destVIPAddr) != 0) {
    uint32_t store = (uint32_t) inet_addr(fwd->destVIPAddr);
    ent->address = store;
    if(fwd->nextHopInterfaceID == current->id) {
        ent->cost = (uint32_t) INFINITY;
    } else {
        ent->cost = (uint32_t) (fwd->cost + 1);
    }
    void *temp = (void *)fwd;
    temp += sizeof(fwd_entry);
    fwd = (fwd_entry *)temp;

    void *etc = (void *)ent;
    etc += sizeof(entry);
    ent = (entry *)etc;
      }
      ent = list;
      int num_entries = (sizeof(fwd_table)/sizeof(fwd_entry));
      construct_and_send_IP(num_entries, ent, 2, current->vipRemote);
      void *temp = (void *)current;
      temp += sizeof(node_interface);
      current = (node_interface *)temp;
    }   
  }
}
int construct_flood_entries(entry *flood_info){
    entry * copyE = flood_info;
    node_interface *entry_pointer = interfaces;

    int k;
    for(k = 0; k < count-1; k++){
        struct in_addr tempIP;
        inet_aton(entry_pointer->vipRemote, &tempIP);
        create_entry(copyE, 1, tempIP.s_addr);
        copyE++;
    }
    return 1;
}

int construct_and_send_IP(int num_entries, entry *RIP_entries_list, int RIPcommand,  char *remoteVIP){
    ip_packet *IPpacket = create_IPpacket_with_RIP(num_entries, RIP_entries_list, RIPcommand, 0, interfaces->vipThis, remoteVIP , 16);    
    char UDPmsg[1400];
    node_interface *temp = table_search(remoteVIP);
    IPtoUDP(IPpacket, UDPmsg);
    send_in_order(sock, temp->ipAddr, temp->port, UDPmsg);
    return 0;
}


int initial_flood(){
    entry flood_info[count-1];
    construct_flood_entries(flood_info);
    node_interface *copy = interfaces;
    int i;
    for (i = 0; i < count-1; i++){
	construct_and_send_IP(count-1, flood_info, 2,  copy->vipRemote);
        copy++;
    }
    return 0;
}

void* next_dest(int idLook){
    node_interface *head = interfaces;
    while(head->id != 0){
        if(head->id == idLook){
            return head;
        }
	head++;
    }
    perror("Next Interface Not Found");
    return NULL;
}

node_interface* table_search(char *remoteVIPs) {
    node_interface *head = interfaces;
    while(strlen(head->ipAddr) != 0){
        if(strcmp(remoteVIPs,head->vipRemote) == 0) {
            return head;
        }
	head++;
    }
    perror("Node Not Found");
    return NULL;
}

int get_src_interface_id(char *srcVIP){
        node_interface *head = interfaces;
    while(head->id != 0){
        if(strcmp(srcVIP,head->vipRemote)){
            return head->id;
        }
    }
    perror("Interface Not Found");
    return -1;
}
 
int routes() {
    //interact with IPRIPInterface to find and print all next-hops; consult table
    fwd_entry *pointer = fwd_table;
    while(strlen(pointer->destVIPAddr) != 0) {
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
    node_interface *pointer = (node_interface *)interfaces;
    while (strlen(pointer->ipAddr) != 0) {
        if ((strcmp(pointer->vipRemote, vipRemote) == 0) && (strcmp(pointer->status, "up") == 0)) {
            //send ipPacket here using port variable (maybe?)
            changeUpDown ("down", pointer->id);
            return 0;
             
        }
        void *temp = (void *)pointer;
        temp += sizeof(node_interface);
        pointer = (node_interface *)temp;
    }
    printf("# Error: specified remote VIP not found or is down\n");
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
            //send_updates(); //triggered updates here
            return 0;
        }
    }
    printf("Interface %d not found\n", id);
    return 1;
}
