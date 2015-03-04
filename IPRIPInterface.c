#include "IPRIPInterface.h"

//for constructing a full nonRIP packet encapsulated in IP
ip_packet *construct_nonRIP_packet_intf(char *message, int id, char *ipAddrSource, char *ipAddrDest, uint8_t ttl) {
	uint32_t ipAddrSrc = (uint32_t) inet_addr(ipAddrSource);
	uint32_t ipAddrDst = (uint32_t) inet_addr(ipAddrDest);
	uint16_t id_in = (uint16_t) id;
	uint8_t ttl_in = (uint8_t) ttl;
	ip_packet *packet = construct_IP_packet(message, id_in, ipAddrSrc, ipAddrDst, ttl_in);
	return packet;
}

//for constructing a full RIP packet encapsulated in IP
ip_packet *construct_RIP_packet_intf(int num_entries, entry *entries, int command, int id, char *ipAddrSource, char *ipAddrDest, uint8_t ttl) { 
	uint16_t command_in = (uint16_t) command;
	uint16_t num_entries_in = (uint16_t) num_entries;
	uint32_t ipAddrSrc = (uint32_t) inet_addr(ipAddrSource);
	uint32_t ipAddrDst = (uint32_t) inet_addr(ipAddrDest);
	uint16_t id_in = (uint16_t) id;
	uint8_t ttl_in = (uint8_t) ttl;
	
	rip_packet *ripPacket = construct_RIP_packet(command_in, num_entries_in, entries);
	char ripPacketChar[sizeof(rip_packet)];
	memcpy(ripPacketChar, ripPacket, sizeof(rip_packet));
	ip_packet *ipPacket = construct_IP_packet(ripPacketChar, id_in, ipAddrSrc, ipAddrDst, ttl_in);
	return ipPacket;
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
	fwd_table = (fwd_entry *)malloc(MAX_ENTRY * sizeof(fwd_entry));
	return 0;
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

