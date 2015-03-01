#include <stdio.h>
#include <string.h>
#include "IP.h"

/*struct ip_packet {
	struct ip header;
	char payload[IP_MAXPACKET - sizeof(struct ip)];
};*/

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
}s