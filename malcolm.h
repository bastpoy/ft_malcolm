#ifndef MALCOLM_H
#define MALCOLM_H

#include "libft.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

#define PORT 219

int fill_addr(struct sockaddr_in *targetaddr, char *addr);

#endif