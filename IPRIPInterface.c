#include "IPRIPInterface.h"

//for constructing a full packet
ip_packet *construct_packet(int num_entries, entry *entries, int command, int id, char *ipAddrSource, char *ipAddrDest, uint8_t ttl) { 
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

int update_fwd_table(char *destIPAddr, int nextHopID, int cost) {
	fwd_entry *fwd_table_pointer = (fwd_entry *)fwd_table;
	while (fwd_table_pointer->destIPAddr != 0) {
		if (strcmp((const char*)fwd_table_pointer->destIPAddr, (const char*)destIPAddr) == 0) {
			if (fwd_table_pointer->cost > cost) {
				fwd_table_pointer->nextHopInterfaceID = nextHopID;
				fwd_table_pointer->cost = cost;
				fwd_table_pointer->last_refresh = time(NULL);
				return 0;
			}
		}
		void *iterator = (void *)fwd_table_pointer;
		iterator += sizeof(fwd_entry);
		fwd_table_pointer = (fwd_entry *)iterator; 
	}
	//entry not found in forwarding table; must add to table
	fwd_entry *ent = (fwd_entry *)fwd_table_pointer;
	memcpy(&ent->destIPAddr, destIPAddr, sizeof(ent->destIPAddr));
	ent->cost = cost;
	ent->last_refresh = time(NULL);
	return 0;
}

