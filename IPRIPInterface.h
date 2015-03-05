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
    char destVIPAddr[32];
    int nextHopInterfaceID; //corresponds to id in node_interface struct
    int cost; 
    time_t last_refresh;
} fwd_entry;

fwd_entry table[MAX_ENTRY * sizeof(entry)];
fwd_entry *fwd_table; 

int create_fwd_table();
int create_entry(entry *newEntry, uint32_t cost, uint32_t address);
int Search_forwarding_table(char * dst_addr);
char *construct_RIP_packet_send(int num_entries, entry *entries, int command);
ip_packet *create_IPpacket_with_RIP(int num_entries, entry *entries, int command, int id, char *ipAddrSource, char *ipAddrDest, uint8_t ttl);
int is_RIP_packet(struct ip *header);
int process_rip_command(rip_packet *RIP);
int update_fwd_table(char destVIPAddr[32], int nextHopID, int cost);

#endif
