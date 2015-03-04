#include "RIP.h"  
  
//implement SHRP  
  
uint16_t command; //1 = request of routing info,   
          //2 = response to routing request  
uint16_t num_entries;  
  
entry *entries; 

rip_packet *construct_RIP_packet(uint16_t command, uint16_t num_entries, entry *entries) {
    rip_packet packet[(2*sizeof(uint16_t)) + (num_entries * sizeof(entry))];
    rip_packet *pointer = packet;
    pointer->command = command;
    pointer->num_entries = num_entries;
    pointer->entries = entries;
    return pointer;

}
 
char *serialize_RIP(uint16_t command, uint16_t num_entries, entry *entries) {
    char *buf = malloc(2 * (sizeof(entries) + sizeof(num_entries))); 
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
        char *command = strsep(&buf, "^"); 
        uint16_t command_conv = (uint16_t) atoi(command); 
 
        char *num_entries = strsep(&buf, "^"); 
        uint16_t num_entries_conv = (uint16_t) atoi(num_entries); 
 
        entry entries[num_entries_conv * sizeof(entry)]; 
        entry *pointer = entries;
    entry *ent = pointer;
    char *cost;
    while ((cost = strsep(&buf, "^")) != NULL) {
        ent->cost = (uint32_t) atoi(cost);
        ent->address = (uint32_t) atoi(strsep(&buf, "^"));
        printf("%d %d\n", ent->cost, ent->address);
        void *temp = (void *)ent;
        temp += sizeof(entry);
        ent = (entry *)temp;        
    }
    rip_packet *ripPacket = construct_RIP_packet(command_conv, num_entries_conv, entries); 
        return ripPacket; 
} 
 
int processPacket() {  
        return 0;  
}