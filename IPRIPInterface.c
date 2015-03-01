#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "RIP.c"
#include "IP.h"

#define INFINITY (16);

typedef struct fwdentry {
	char *destIPAddr[32];
	char *nextHopIP[32];
	int cost; 
	time_t last_refresh;
} fwd_entry;

struct fwd_entry *fwd_table; 

ip_packet *construct_IP_packet_IP(rip_packet *ripPacket, uint16_t id, uint32_t ipAddrSrc, uint32_t ipAddrDest, uint8_t ttl) {
	ip_packet *ipPacket = (ip_packet *)malloc(sizeof(ip_packet));
	ip_packet *pointer = ipPacket;
	memcpy(pointer->payload, ripPacket, sizeof(pointer->payload));

	pointer->header.ip_id = id;
	void *pointer_src = &pointer->header.ip_src;
	memcpy(pointer_src, &ipAddrSrc, sizeof(pointer->header.ip_src));
	void *pointer_dst = &pointer->header.ip_dst;
	memcpy(pointer_dst, &ipAddrDest, sizeof(pointer->header.ip_dst));

	char *header_char = (char *)malloc(sizeof(struct ip));
	memcpy(header_char, &pointer->header, sizeof(struct ip));
	pointer->header.ip_sum = ip_sum(header_char, (int) sizeof(struct ip));

	pointer->header.ip_ttl = ttl; 

	return ipPacket;
}

ip_packet *construct_IP_packet(int num_entries, struct entry *entries, int command, int id, char *ipAddrSource, char *ipAddrDest, uint8_t ttl) { 
	uint16_t command_in = (uint16_t) command;
	uint16_t num_entries_in = (uint16_t) num_entries;
	uint32_t ipAddrSrc = (uint32_t) inet_addr(ipAddrSource);
	uint32_t ipAddrDst = (uint32_t) inet_addr(ipAddrDest);
	uint16_t id_in = (uint16_t) id;
	uint8_t ttl_in = (uint8_t) ttl;
	
	rip_packet *ripPacket = construct_RIP_packet(command_in, num_entries_in, entries);
	ip_packet *ipPacket = construct_IP_packet_IP(ripPacket, id_in, ipAddrSrc, ipAddrDst, ttl_in);
	return ipPacket;
}

int update_fwd_table(char *destIPAddr, char *nextHopIP, int cost) {
	fwd_entry *fwd_table_pointer = (fwd_entry *)fwd_table;
	while (fwd_table_pointer->destIPAddr != 0) {
		if (strcmp((const char*)fwd_table_pointer->destIPAddr, (const char*)destIPAddr) == 0) {
			if (fwd_table_pointer->cost > cost) {
				memcpy(fwd_table_pointer->nextHopIP, nextHopIP, sizeof(fwd_table_pointer->nextHopIP));
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

