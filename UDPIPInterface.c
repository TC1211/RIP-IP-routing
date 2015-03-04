#include "UDPIPInterface.h"

int set_up_recv_sock(int *sock, char *addr, uint16_t port, char *received_packet) {
	struct sockaddr_in receivedfrom_addr;
	recv_sockaddr(sock,addr, port, &receivedfrom_addr);
	sock_recv(sock, &receivedfrom_addr, received_packet);
	return 0;
}

//Enforce Host Byte Order
int UDPtoIP(char *packet, ip_packet *IPpack) {
	
	char *ttl;	
	char token = '\n';
	ttl = strtok(packet ,&token);
	IPpack->header.ip_ttl = ntohs(atoi(ttl));

	printf("ttl: %d \n", (int)IPpack->header.ip_ttl);

	ttl = strtok(NULL ,&token);
	ttl = strtok(NULL ,&token);
	IPpack->header.ip_p = ntohs(atoi(ttl));
	printf("%d \n", IPpack->header.ip_p);

	ttl = strtok(NULL ,&token);
	ttl = strtok(NULL ,&token);
	IPpack->header.ip_sum = ntohs(atoi(ttl));
	printf("%d \n", IPpack->header.ip_sum);

	ttl = strtok(NULL ,&token);
	ttl = strtok(NULL ,&token);
	IPpack->header.ip_id = ntohs(atoi(ttl));
	printf("%d \n", IPpack->header.ip_id);


	ttl = strtok(NULL ,&token);
	ttl = strtok(NULL ,&token);
	printf("%s \n", ttl);
	inet_aton(ttl, &(IPpack->header.ip_src));

	ttl = strtok(NULL ,&token);
	ttl = strtok(NULL ,&token);
	printf("%s \n", ttl);
	inet_aton(ttl, &(IPpack->header.ip_dst));
	
	ttl = "";
	ttl = strtok(NULL ,&token);
	ttl = strtok(NULL, &token);
	char *tempMsg = malloc(sizeof(ttl));
	strcpy(tempMsg, ttl);
	IPpack->payload = tempMsg;
	printf("%s \n", IPpack->payload);

	return 0;
}

//Enforce Network Byte Order
int IPtoUDP(ip_packet *IPpack, char *UDPmsg) {
	char pad = '\n';
	char *buf = UDPmsg;
	char *ttl = malloc(8);
	sprintf(ttl, "%i", htons(IPpack->header.ip_ttl));
	strcat(ttl,&pad);	
	memcpy(buf, ttl, strlen(ttl));

	char *prot = malloc(8);
	sprintf(prot, "%i", htons(IPpack->header.ip_p));
	strcat(prot,&pad);
	strcat(buf,prot);

	char *sum = malloc(8);
	sprintf(sum, "%i", htons(IPpack->header.ip_sum));
	strcat(sum,&pad);
	strcat(buf,sum);


	char *id = malloc(8);
	sprintf(id, "%i", htons(IPpack->header.ip_id));
	strcat(id,&pad);
	strcat(buf,id);

	char * src_addr = inet_ntoa(IPpack->header.ip_src);
	strcat(src_addr,&pad);
	strcat(buf,src_addr);

	char * dst_addr = inet_ntoa(IPpack->header.ip_dst);
	strcat(dst_addr,&pad);
	strcat(buf,dst_addr);
	
	strcat(buf, IPpack->payload);
	printf("%s\n\n", buf);
	return 0;
}
 

int fragment_large_packets() {
	return 0;
}

 

int assemble_broken_fragments() {
	return 0;
}

 

int send_in_order() {
	return 0;
}
