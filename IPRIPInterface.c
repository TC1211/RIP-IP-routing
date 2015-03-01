#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include "RIP.c"
#include "IP.c"

#define INFINITY (16);

struct fwd_entry {
	char *destIPAddr[32];
	char *nextHopIP[32];
	int cost; 
	time_t last_refresh;
}

struct fwd_entry *fwd_table[1024]; 

struct ip_packet *construct_IP_packet(int num_entries, struct entry *entries, int command, int id, char ipAddrSource, char ipAddrDest, uint8_t ttl) { 
	uint16_t command_in = (uint16_t) command;
	uint16_t num_entries_in = (uint16_t) num_entries;
	uint32_t ipAddrSrc = (uint32_t) inet_addr(ipAddrSource);
	uint32_t ipAddrDst = (uint32_t) inet_addr(ipAddrDest);
	uint16_t id_in = (uint16_t) id;
	uint8_t ttl_in = (uint8_t) ttl;
	
	struct rip_packet *ripPacket = construct_RIP_packet(command_in, num_entries_in, entries);
	struct ip_packet *ipPacket = construct_ip_packet(ripPacket, id_in, ipAddrSrc, ipAddrDst, ttl_in);
	return ipPacket;
}

int update_fwd_table(char *destIPAddr, char *nextHopIP, int cost) {
	struct fwd_entry *fwd_table_pointer = fwd_table;
	while (fwd_table_pointer->destIPAddr != 0) {
		if (fwd_table_pointer->destIPAddr == destIPAddr) {
			if (fwd_table_pointer->cost > cost) {
				fwd_table_pointer->nextHopIP = nextHopIP;
				fwd_table_pointer->cost = cost;
				fwd_table_pointer->last_refresh = time(NULL);
				return 0;
			}
		}
		fwd_table_pointer++;
	}
	//entry not found in forwarding table; must add to table
	struct fwd_entry *ent = (fwd_entry *)fwd_table_pointer;
	ent->destIPAddr = destIPAddr;
	ent->cost = cost;
	ent->last_refresh = time(NULL);
	return 0;
}

