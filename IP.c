#include <stdio.h>
#include "ipsum.c"
#include "IPRIPInterface.c"
#include "IPUDPInterface.c"
#include <ip.h>

//recalculate checksum
//decrement TTL

struct iphdr *ip, *ipreply;
int TTL;
int checksum;

int main() {
	checksum = ip_sum(message, random integer!?);
}
