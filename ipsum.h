#ifndef IPSUM_H
#define IPSUM_H

#include <inttypes.h>

//do an ip checksum on a generic block of memory
//for IP, len should always be the size of the ip header (sizeof (struct ip))
extern int ip_sum(char* packet, int len);

#endif
