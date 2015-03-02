#ifndef IP_H
#define IP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include </usr/include/netinet/ip.h>
#include "ipsum.h"

typedef struct ip_packet {
	struct ip header;
	char payload[IP_MAXPACKET - sizeof(struct ip)];
} ip_packet; 

ip_packet *construct_IP_packet(char *ripPacket, uint16_t id, uint32_t ipAddrSrc, uint32_t ipAddrDest, uint8_t ttl);
int is_RIP(struct ip *header);
int process_header_for_forwarding(struct ip *header);

#endif
