#include "IP.h"

//for attaching IP header to a packet
ip_packet *construct_IP_packet(char *ripPacket, uint16_t id, uint32_t ipAddrSrc, uint32_t ipAddrDest, uint8_t ttl) {
	ip_packet *ipPacket = (ip_packet *)malloc(sizeof(ip_packet));
	ip_packet *pointer = ipPacket;

	unsigned long int temp;
	if(sizeof(ripPacket) >= sizeof(ipPacket->payload)) temp = sizeof(ipPacket->payload);
	else temp = sizeof(ripPacket);
	memcpy(pointer->payload, ripPacket, temp); 

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
	int ttl = (int) header->ip_ttl;
	ttl -= 1;
	unsigned char ttl_uchar = (unsigned char) ttl;
	header->ip_ttl = ttl_uchar;

	//Recompute Checksum
	char *header_char = (char *)malloc(sizeof(struct ip));
	memcpy(header_char, header, sizeof(struct ip));
	header->ip_sum = ip_sum(header_char, (int) sizeof(struct ip));
	return 0;
}
