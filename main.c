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
int ifconfig(); //ifconfig command 
int changeUpDown(); //changeUpDown command 
void *receive_func(void *arg); 
char *construct_SHRP_packet(int id, char *ipAddrSource, char *ipAddrSHRP); //creates  
    //RIP packet with poison reverse; must be called for each interface that is up 
int send_RIP_response(char *ipAddrDest); //send RIP response packet to whoever send you a  
    //request, NOT to all of your neighbors 
int send_RIP_request(int id, char *ipAddrSrc, char *ipAddrDst); 
int routes(); //routes command 
int send_updates(); //to be called for sending either triggered updates or periodic updates 
int send_message(char *vipRemote, char *message); //create and send message (non-RIP) 
int test_send(); 
int receive_RIP_packet(rip_packet *packet); 
void* next_dest(int);
 
 
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
        if((result = pthread_create(&children_tid[i], NULL, (void *)receive_func,arg_list))){ 
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
            send_updates(); //triggered updates here 
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
     
    ip_packet *IPpacket=(ip_packet *)malloc(sizeof(ip_packet)); 
    UDPtoIP(args->received_packet, IPpacket); 
     // CHECK THE HEADER FIRST
	struct in_addr remote = IPpacket->header.ip_dst;
	char *remoteVIP = inet_ntoa(remote);
	if (strcmp(remoteVIP, interfaces->vipThis)){
		if (is_RIP_packet(&IPpacket->header)) { 
		printf("Is RIP packet. DEAL WITH IT\n");
		} else {
			printf("%s", IPpacket->payload);
		}
	} else {
		int id = Search_forwarding_table(remoteVIP);
		struct node_interface *node = next_dest(id);
		char UDPmsg[1400];
		IPtoUDP(IPpacket, UDPmsg);
		send_in_order(sock, node->ipAddr, node->port, UDPmsg);
    	} 
      
      pthread_exit(NULL); 
} 

void* next_dest(int idLook){
	node_interface *head = interfaces;
	while(head->id != 0){
		if(head->id == idLook){
			return head;
		}
	}
	perror("Next Interface Not Found");
	return NULL;
}

char *construct_SHRP_packet(int id, char *ipAddrSource, char *ipAddrSHRP) { 
    //for each interface, if nexthop in fwd_table is ipAddrSHRP, use cost = INFINITY 
    entry *entries = (entry *)malloc(MAX_ENTRY * sizeof(entry)); //RIP entries 
    int num_entries = 0; 
     
    entry *entry_pointer = (entry *)entries; 
    node_interface *interface_pointer = (node_interface *)interfaces; 
    fwd_entry *fwd_pointer; 
 
    //find interface ID corresponding to ipAddrSHRP: 
    node_interface *shrp = (node_interface *)interfaces; 
    while (strcmp(shrp->ipAddr, ipAddrSHRP) != 0) { 
        void *temp = (void *)shrp; 
        temp += sizeof(node_interface); 
        shrp = (node_interface *)temp; 
    } 
    int idSHRP = shrp->id; //interface ID for all routes with next hop = SHRP node 
     
    //for each interface: 
    while (interface_pointer->id != 0) { 
        //find corresponding fwd_table entry: 
        fwd_pointer = (fwd_entry *)fwd_table; 
        while (fwd_pointer->nextHopInterfaceID != interface_pointer->id) { 
            void *temp = (void *)fwd_pointer; 
            temp += sizeof(fwd_entry); 
            fwd_pointer = (fwd_entry *)temp; 
        } 
        //if interface is up: 
        if (strcmp(interface_pointer->status, "up") == 0) { 
            //if fwd table entry's next hop interface ID == SHRP interface ID: 
            if (fwd_pointer->nextHopInterfaceID == idSHRP) { 
                //set cost to infinity 
                entry_pointer->cost = INFINITY; 
            } else { 
                //otherwise set cost to (current cost) + 1 
                entry_pointer->cost = fwd_pointer->cost + 1; 
            } 
            //store the remote VIP address in entry as well 
            entry_pointer->address = (uint32_t) inet_addr(fwd_pointer->destVIPAddr); 
             
            //increment current place in the list of entries 
            void *temp = (void *)entry_pointer; 
            temp += sizeof(entry); 
            entry_pointer = (entry *)temp; 
             
            //increment number of entries 
            num_entries++; 
        } 
         
        void *temp = (void *)interface_pointer; 
        temp += sizeof(node_interface); 
        interface_pointer = (node_interface *)temp; 
    } 
     
    char *ipPacket = create_IPpacket_with_RIP(num_entries, entries, 2, id, ipAddrSource, ipAddrSHRP, INFINITY);  
    return ipPacket; 
} 
 
int send_RIP_response(char *ipAddrDest) { 
    //send a SHRP RIP packet to whoever sent you the request 
    node_interface *pointer = (node_interface *)interfaces; 
    while (strcmp(pointer->ipAddr, ipAddrDest) != 0) { 
        void *temp = (void *)pointer; 
        temp += sizeof(node_interface); 
        pointer = (node_interface *)temp; 
    } 
    char *shrpPacket = construct_SHRP_packet(pointer->id, ipAddrThis, ipAddrDest);  
 
    //send packets here here 
 
    return 0; 
} 
 
int send_RIP_request(int id, char *ipAddrSrc, char *ipAddrDst) { 
    entry *ent = (entry *)malloc(sizeof(ent)); 
    memset(ent, 0, sizeof(ent)); 
    int num_entries = 0;  
    char *packet = create_IPpacket_with_RIP(num_entries, ent, 1, id, ipAddrSrc, ipAddrDst, INFINITY);  
     
    //send ipPacket here  
 
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
 
int send_updates() { 
    node_interface *pointer = (node_interface *)interfaces; 
    while (strlen(pointer->ipAddr) != 0) { 
        int port = pointer->port; 
        char *shrpPacket = construct_SHRP_packet(pointer->id, ipAddrThis, pointer->ipAddr);  
 
        //send packet here using port variable (maybe?) 
 
        void *temp = (void *)pointer; 
        temp += sizeof(node_interface); 
        pointer = (node_interface *)temp; 
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
 
int test_send() { 
    ip_packet sendIP; 
    sendIP.header.ip_ttl = 16; 
    sendIP.header.ip_p = 200; 
    sendIP.payload = "moe moe moe"; 
    char *UDP = malloc(1400); 
    IPtoUDP(&sendIP, UDP); 
    sock_send(sock, "127.0.0.1", 17001, UDP);

    entry *one = (entry *)malloc(sizeof(entry));
    one->cost = 10;
    one->address = (uint32_t) inet_addr("10.10.168.73");
    entry *two = (entry *)malloc(sizeof(entry));
    two->cost = 11;
    two->address = (uint32_t) inet_addr("10.10.98.12");
    entry *final = (entry *)malloc(2*sizeof(entry));
    entry *list = final;
    memcpy(list, one, sizeof(entry));
    void *tmp = (void *)list;
    tmp += sizeof(entry);
    list = (entry *)tmp;
    memcpy(list, two, sizeof(entry));
    char *buf = serialize_RIP(1, 2, final);
    rip_packet *packet = deserialize_RIP(buf); 
    return 0; 
} 

int initial_flood(){
	entry flood_info[count-1];
	entry * copyE = flood_info;
	node_interface *entry_pointer = interfaces;

	int k;
	for(k = 0; k < count-1; k++){
		struct in_addr tempIP;
		inet_aton(entry_pointer->vipRemote, &tempIP);
		create_entry(copyE, 1, tempIP.s_addr);
		copyE++;
	}

	node_interface *copy = interfaces;
	char *UDPmsg = (char *)malloc(sizeof(char)*1400);

	int i;

	for (i = 0; i < count-1; i++){
		ip_packet *packet_to_send = create_IPpacket_with_RIP(count, flood_info, 2, 0, copy->vipThis, copy->vipRemote , 16);
		IPtoUDP(packet_to_send, UDPmsg);
		send_in_order(sock, copy->ipAddr, copy->port, UDPmsg);
		copy++;
	}
	return 0;
}
 
int receive_RIP_packet(rip_packet *packet) { 
    printf("received info: %d\t%d\n", packet->command, packet->num_entries); 
    if (packet->command == 1) { //request 
         
    } else if (packet->command == 2) { //response 
 
    } else { 
        printf("Error in RIP packet input; command not valid\n");     
        return 1; 
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
    initial_flood();
    //printf("**testing test_send:\n"); 
    //test_send(); 
    create_fwd_table(); 
    int i = 0; 
    for(i = 0; i < count - 1; i++) { 
        update_fwd_table(interfaces[i].vipRemote, interfaces[i].id, INFINITY); 
//        send_RIP_request(interfaces[i].id, ipAddrThis, interfaces[i].ipAddr); 
    } 
     
/*    printf("\n**testing ifconfig command:\n"); 
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
 
    //create thread that checks repeatedly for outdated entries 
    sleep(1); 
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