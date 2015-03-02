#ifndef IPRIPINTERFACE_H
#define IPRIPINTERFACE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "RIP.h"
#include "IP.h"

#define INFINITY 16
#define MAX_ENTRY 1024

typedef struct fwd_entry {
	char *destIPAddr;
	int nextHopInterfaceID;
	int cost; 
	time_t last_refresh;
} fwd_entry;

fwd_entry *fwd_table; 

int create_fwd_table();
ip_packet *construct_packet(int num_entries, entry *entries, int command, int id, char *ipAddrSource, char *ipAddrDest, uint8_t ttl);
int update_fwd_table(char *destIPAddr, int nextHopID, int cost);

#endif
