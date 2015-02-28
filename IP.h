struct IPpacket{
	struct ip header;
	char payload[IP_MAXPACKET-sizeof(struct ip)];
}

int is_RIP(struct ip *header);
int process_header_for_forwarding(struct ip *header);
