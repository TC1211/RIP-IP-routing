#include <stdint.h>
#include "RIP.c"
#include "IP.c"

//assemble RIP packet--combine command (uint16_t) with the entries array
//CREATE STRUCT

struct ip_packet {
	uint16_t command;
	uint16_t num_entries;
	struct entry *entries;
};

struct ip_packet packet;

int constructPacket(uint16_t command, uint16_t num_entries, struct entry *entries) {
	packet.command = command;
	packet.num_entries = num_entries;
	strcpy(packet.entries, entries);
}
