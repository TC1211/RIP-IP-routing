#include "RIP.h"

//implement SHRP

uint16_t command; //1 = request of routing info, 
		  //2 = response to routing request
uint16_t num_entries;

entry *entries;

rip_packet *construct_RIP_packet(uint16_t command, uint16_t num_entries, entry *entries) {
	rip_packet *ripPacket = (rip_packet *)malloc(sizeof(rip_packet));
	rip_packet *pointer = ripPacket;
	pointer->command = command;
	pointer->num_entries = num_entries;
	pointer->entries = entries;
	memcpy(pointer->entries, entries, sizeof(pointer->entries));

	return ripPacket;
}

int processPacket() {
	return 0;
}



