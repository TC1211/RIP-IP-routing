#include "RIP.h" 
 
//implement SHRP 
 
uint16_t command; //1 = request of routing info,  
          //2 = response to routing request 
uint16_t num_entries; 
 
entry *entries; 
 
char *serialize_RIP(uint16_t command, uint16_t num_entries, entry *entries) {
    char *buf = malloc(2 * (sizeof(entries) + sizeof(num_entries)));;
    char *delimiter = "^";

    char command_conv[16];
    sprintf(command_conv, "%d", command);
    char num_entries_conv[16];
    sprintf(num_entries_conv, "%d", num_entries);
    strcpy(buf, command_conv);
    strcat(buf, delimiter);
    strcat(buf, num_entries_conv);

    entry *iterator = entries;
    while (iterator->address != 0) {
        char address[32];
        char cost[32];
        sprintf(cost, "%d", iterator->cost);
        sprintf(address, "%d", iterator->address);
        
        strcat(buf, delimiter);
        strcat(buf, cost);
        strcat(buf, delimiter);
        strcat(buf, address);

        void *temp = (void *)iterator;
        temp += sizeof(entry);
        iterator = (entry *)temp;
    }
    printf("buf: %s\n", buf);
    return buf;
}

rip_packet *deserialize_RIP(char *buf) {
    rip_packet *ripPacket = (rip_packet *)malloc(2 * sizeof(buf));
    char *command = strtok(buf, "^");
    uint32_t command_conv = (uint32_t) atoi(command);
    ripPacket->command = command_conv;
    printf("command: %d\n", ripPacket->command);

    char *num_entries = strtok(NULL, "^");
    uint32_t num_entries_conv = (uint32_t) atoi(num_entries);
    ripPacket->num_entries = num_entries_conv;
    printf("num_entries: %d\n", ripPacket->num_entries);

    entry *entries = (entry *)malloc(sizeof(buf));
    entry *pointer = entries;
    char *cost, *address;
    cost = strtok(NULL, "^");
    return ripPacket;
} 

/*char *construct_RIP_buffer(uint16_t command, uint16_t num_entries, entry *entries) { 
    rip_packet *ripPacket = (rip_packet *)malloc(sizeof(rip_packet)); 
    rip_packet *pointer = ripPacket; 
    pointer->command = command; 
    pointer->num_entries = num_entries; 
    pointer->entries = entries; 
    memcpy(pointer->entries, entries, sizeof(pointer->entries)); 
    
    char *buf = serialize_entries(command, num_entries, entries);
    printf("final buffer: %s\n", buf); 
    return buf; 
}*/

 
int processPacket() { 
    return 0; 
}