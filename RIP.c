#include <stdio.h>
#include <stdint.h>
#include "IPRIPInterface.c"

//implement SHRP

uint16_t command; //1 = request of routing info, 
		  //2 = response to routing request
uint16_t num_entries; //max 64, must be 0 for request

struct entry {
	uint32_t cost; //max 16
	uint32_t address;
} entries[num_entries];

int processPacket() {
	
}

int parse(*entry) {

}

sendToInterface() {
	constructPacket(command, num_entries, *entries);
}
