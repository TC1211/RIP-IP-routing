typedef struct _entry {
	uint32_t cost; //max 16
	uint32_t address;
} entry;

typedef struct _rip_packet {
	uint16_t command;
	uint16_t num_entries;
	struct entry *entries;
} rip_packet;

rip_packet *construct_RIP_packet(uint16_t command, uint16_t num_entries, struct entry *entries);

int processPacket();
