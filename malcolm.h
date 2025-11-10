#ifndef MALCOLM_H
#define MALCOLM_H

#include "libft.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <stdbool.h>
// #include <linux/if_arp.h>



#define PORT 219

struct arp_header {
    unsigned short hardware_type; // 2 bytes => type of network on which ARP is running => 1 for arp
    unsigned short protocol_type; // 2 bytes => protocol type ipv4 for example
    unsigned char hardware_len; // 1 bytes => length of the pysical adress
    unsigned char protocol_len; // 1 bytes => defining the length of the logical adress in bytes. for ipv4 it is 4 for 4bytes.   
    unsigned short opcode; // 2 bytes => type of arp request for arp request and reply
    unsigned char sender_mac[6]; // 6 bytes ethernet adress 
    unsigned char sender_ip[4]; // 4 bytes ip adress
    unsigned char target_mac[6]; // 6 bytes ethernet adress
    unsigned char target_ip[4]; // 4 bytes ip adress
}; // 28 bytes

// sizeof ethhdr 14 bytes => total 42 bytes

int fill_addr(struct sockaddr_in *targetaddr, char *addr);
void print_uchar(unsigned char *array);
void print_mac(unsigned char *mac);

#endif