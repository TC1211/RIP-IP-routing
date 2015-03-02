#ifndef UDPIPINTERFACE_H
#define UDPIPINTERFACE_H

#include <stdint.h>
#include "IP.h"
#include "UDPIPInterface.h"
#include "UDPSocket.h"

int set_up_recv_sock(int *sock,char *addr, uint16_t port, char *received_packet);
int UDPtoIP(char *packet);
int fragment_large_packets();
int assemble_broken_fragments();
int send_in_order();

#endif
