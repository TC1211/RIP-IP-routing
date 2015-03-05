#ifndef IP_H
#define IP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include </usr/include/netinet/ip.h>
#include "ipsum.h"

typedef struct ip_packet {
	struct ip header;  // VIP ADDRESSES ONLY!!!!!!!!!!!!
	char *payload;
} ip_packet; 

ip_packet *construct_IP_packet(char *packet, uint16_t id, uint32_t ipAddrSrc, uint32_t ipAddrDest, uint8_t ttl, uint8_t ip_p);
int is_RIP(struct ip *header);
int process_header_for_forwarding(struct ip *header);
int checksum_compute(ip_packet *IP);
#endif
