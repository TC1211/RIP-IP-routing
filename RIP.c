#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define num_entries_default (64);

//implement SHRP

uint16_t command; //1 = request of routing info, 
		  //2 = response to routing request
uint16_t num_entries = num_entries_default;

typedef struct _entry {
	uint32_t cost; //max 16
	uint32_t address;
} entry;

typedef struct _rip_packet {
	uint16_t command;
	uint16_t num_entries;
	struct entry *entries;
} rip_packet;

entry *entries;

rip_packet *construct_RIP_packet(uint16_t command, uint16_t num_entries, struct entry *entries) {
	rip_packet *ripPacket = (rip_packet *)malloc(sizeof(rip_packet));
	rip_packet *pointer = ripPacket;
	pointer->command = command;
	pointer->num_entries = num_entries;
	pointer->entries = entries;
	memcpy(pointer->entries, entries, sizeof(pointer->entries));
//	strcpy(pointer->entries, entries);

	return ripPacket;
}

int processPacket() {
	return 0;
}



