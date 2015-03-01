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
	header->ip_ttl = (u_char) ((int)header->ip_ttl-1);
	//Recompute Checksum
	header->ip_sum = ip_sum(header, sizeof(struct ip));
	return 0;
}
