#include <stdlib.h>
#include </usr/include/netinet/ip.h>
#include "ipsum.h"

struct ip_packet{
	struct ip header;
	char payload[IP_MAXPACKET - sizeof(struct ip)];
}

struct ip_packet *construct_IP_packet(struct rip_packet ripPacket, uint16_t id, uint32_t ipAddrSrc, uint32_t ipAddrDest, uint8_t ttl) {
	struct ip_packet *ipPacket;
	struct ip_packet *pointer;
	strcpy(pointer->payload, ripPacket);
	pointer->header->ip_id = id;
	strcpy(pointer->header->ip_src, ipAddrSrc);
	strcpy(pointer->header->ip_dst, ipAddrDest);

	pointer->header->ip_sum = ip_sum(header, sizeof(struct ip));
	pointer->header->ip_ttl = ttl; 

	return ip_packet;
}

int is_RIP(struct ip *header) {
	//check whether to deliver to RIP
	if(header->ip_p == 200) { //RIP
		return 1;
	} else {
		return 0;
	}
}

int process_header_for_forwarding(struct ip *header) {
	//Decrement TTL
	header->ip_ttl = (u_char) ((int)header->ip_ttl-1);
	//Recompute Checksum
	header->ip_sum = ip_sum(header, sizeof(struct ip));
	return 0;
}
