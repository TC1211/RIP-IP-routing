#include "IPRIPInterface.h"

//for constructing a full RIP packet encapsulated in IP
char *construct_RIP_packet_send(int num_entries, entry *entries, int command) { 
    uint16_t command_in = (uint16_t) command;
    uint16_t num_entries_in = (uint16_t) num_entries;
  
    char *ripBuf = serialize_RIP(command_in, num_entries_in, entries);
    return ripBuf;
}

ip_packet *create_IPpacket_with_RIP(int num_entries, entry *entries, int command, int id, char *ipAddrSource, char *ipAddrDest, uint8_t ttl){
	uint32_t ipAddrSrc = (uint32_t) inet_addr(ipAddrSource);
    	uint32_t ipAddrDst = (uint32_t) inet_addr(ipAddrDest);
    	uint16_t id_in = (uint16_t) id;
    	uint8_t ttl_in = (uint8_t) ttl;

	char *RIP_serialized = construct_RIP_packet_send(num_entries, entries, command);	

	ip_packet *IPpacket_with_RIP = construct_IP_packet(RIP_serialized, id_in, ipAddrSrc, ipAddrDst, ttl_in);
	
	return IPpacket_with_RIP;
}

int process_rip_command(rip_packet *RIP){
	int command = RIP->command;
	if(command == 1){
		return 1;
	}
	return 0;
}

int create_entry(entry *newEntry, uint32_t cost, uint32_t address){
    newEntry-> cost = cost;
    newEntry->address=address;
    return 0;
}

int is_RIP_packet(struct ip *header) { 
    return is_RIP(header);
}

int create_fwd_table() {
  fwd_table = table;
    return 0;
}

int Search_forwarding_table(char * dst_addr){
	fwd_entry * head = fwd_table;
	int i = 0;
	while(head->cost != 0 && i < MAX_ENTRY){
		if(strcmp(head->destVIPAddr,dst_addr)){
			return head->nextHopInterfaceID;
		}
		head++;
		i++;
	}
	perror("Entry not found");
	return -1;
}

int update_fwd_table(char destVIPAddr[32], int nextHopID, int cost) {
    fwd_entry *pointer = (fwd_entry *)fwd_table;
    while (strlen(pointer->destVIPAddr) != 0) {
        if (strcmp(pointer->destVIPAddr, destVIPAddr) == 0) {
            if (pointer->cost > cost) {
                pointer->nextHopInterfaceID = nextHopID;
                pointer->cost = cost;
                pointer->last_refresh = time(NULL);
                return 0;
            }
        }
        void *temp = (void *)pointer;
        temp += sizeof(fwd_entry);
        pointer = (fwd_entry *)temp; 
    }
    //entry not found in forwarding table; must add to table    
    fwd_entry *ent = (fwd_entry *)pointer;
    memcpy(&ent->destVIPAddr, (void *)destVIPAddr, sizeof(ent->destVIPAddr));
    ent->nextHopInterfaceID = nextHopID;
    ent->cost = cost;
    ent->last_refresh = time(NULL);
    return 0;
}
