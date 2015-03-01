#include <stdlib.h>
#include </usr/include/netinet/ip.h>
#include "ipsum.h"

typedef struct _ip_packet {
	struct ip header;
	char payload[IP_MAXPACKET-sizeof(struct ip)];
} ip_packet; 

int is_RIP(struct ip *header);
int process_header_for_forwarding(struct ip *header);
