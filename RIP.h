#ifndef RIP_H
#define RIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define num_entries_default 64

typedef struct entry {
	uint32_t cost; //max 16
	uint32_t address;
} entry;

typedef struct rip_packet {
	uint16_t command;
	uint16_t num_entries;
	struct entry *entries;
} rip_packet;

rip_packet *construct_RIP_packet(uint16_t command, uint16_t num_entries, struct entry *entries);
int processPacket();

#endif
