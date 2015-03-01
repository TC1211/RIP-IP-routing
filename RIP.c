#include <stdio.h>
#include <stdint.h>

//implement SHRP

uint16_t command; //1 = request of routing info, 
		  //2 = response to routing request
uint16_t num_entries; //max 64, must be 0 for request

struct _entry {
	uint32_t cost; //max 16
	uint32_t address;
} entry;

struct _rip_packet {
	uint16_t command;
	uint16_t num_entries;
	struct entry *entries[1024];
} rip_packet;

struct entry *entries[num_entries];
struct rip_packet *construct_RIP_packet(uint16_t command, uint16_t num_entries, struct entry *entries) {
	struct rip_packet *ripPacket;
	struct rip_packet *pointer = ripPacket;
	pointer->command = command;
	
	pointer->num_entries = num_entries;
	strcpy(pointer->entries, entries);
	return ripPacket;
}

int processPacket() {
	return 0;
}

int parse(*entry) {
	return 0;
}



