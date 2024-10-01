#include "malcolm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

void print_mac(unsigned char *mac) {
    printf("%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

int main(int argc, char *argv[]) 
{
    (void) argc;
    (void) argv;
    // struct sockaddr_in rec;
    // struct sockaddr_in source, dest;
    struct ifaddrs *ifaddr, *ifa ;
    struct sockaddr_ll *s;
    char buffer[65536];
    int sockfd;
    // int broadcast = 1;
    // char buffer1[1024];
    // char buffer2[1024];
    
    struct sockaddr_in *target;
    // socklen_t sin_size = sizeof(struct sockaddr);
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(sockfd < 0)
    {
        printf("error socket: %s",strerror(errno));
        return(errno);
    }

    // fill_addr(&targetaddr, argv[1]);
    // fill_addr(&srcaddr, argv[2]);

    if(getifaddrs(&ifaddr) == -1)
    {
        printf("error on getting addr: %s\n", strerror(errno));
        return(errno);
    }
    for(ifa = ifaddr; ifa; ifa = ifa->ifa_next)
    {
        if(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && ft_strnstr(ifa->ifa_name, "enx", ft_strlen(ifa->ifa_name)))
        {
            target = (struct sockaddr_in*)ifa->ifa_addr;
            inet_ntop(AF_INET, &target->sin_addr, buffer, 1024);
            printf("Interface : %s \t Address: %s\n", ifa->ifa_name, buffer);
        }
        if (ifa->ifa_addr->sa_family == AF_PACKET && ifa->ifa_data && ft_strnstr(ifa->ifa_name, "enx", ft_strlen(ifa->ifa_name))) {
            s = (struct sockaddr_ll *)ifa->ifa_addr;
            printf("Interface MAC: %s \t MAC: ", ifa->ifa_name);
            for (int i = 0; i < s->sll_halen; i++) {
                printf("%x%s", (unsigned char)s->sll_addr[i], (i + 1 != s->sll_halen) ? ":" : "");
            }
            printf("\n");
        }
    }

    printf("sizeof ethhdr %ld\n", sizeof(struct ethhdr));
    printf("listening...\n");
    while(1) {
        ssize_t packet_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);

        if(packet_len == -1) {
            perror("Packet receive failed");
            continue;
        }

        struct ethhdr *eth = (struct ethhdr *)buffer;
        if(ntohs(eth->h_proto) == ETH_P_ARP) 
        {
            struct arp_header *arp = (struct arp_header *)(buffer + sizeof(struct ethhdr));
            if(arp->sender_ip[0] == 192 && arp->sender_ip[1] == 168 && arp->sender_ip[2] == 0 && arp->sender_ip[3] == 55)
            {
                if(eth->h_dest[0] == 0xFF && eth->h_dest[1] == 0xFF && eth->h_dest[2] == 0xFF &&
                    eth->h_dest[3] == 0xFF && eth->h_dest[4] == 0xFF && eth->h_dest[5] == 0xFF)
                {
                    // Receiving packet
                    printf("ARP packet received:\n");
                    printf("  Source MAC: ");
                    print_mac(eth->h_source);
                    printf("\n  Dest MAC: ");
                    print_mac(eth->h_dest);
                    printf("\n  Sender IP: %d.%d.%d.%d\n", arp->sender_ip[0], arp->sender_ip[1], arp->sender_ip[2], arp->sender_ip[3]);
                    printf("  Target IP: %d.%d.%d.%d\n", arp->target_ip[0], arp->target_ip[1], arp->target_ip[2], arp->target_ip[3]);
                    
                    // sending packet
                    unsigned char buffersend[42];
                    struct ethhdr *ethsend = (struct ethhdr *)buffersend;
                    struct arp_header *arpsend = (struct arp_header *)(buffer + sizeof(struct ethhdr));

                    //eth header
                    ethsend->h_proto = htons(ETH_P_AARP);
                    ft_memcpy(ethsend->h_dest, ethsend->h_source, ETH_ALEN);
                    ft_memcpy(ethsend->h_source, s->sll_addr, ETH_ALEN);
                    
                    //arp header
                    arpsend->hardware_type = htons(1);
                    arpsend->protocol_type = htons(ETH_P_AARP);
                    arpsend->hardware_len = 6; // mac length
                    arpsend->protocol_len = 4; // ip length
                    arpsend->opcode = htons(0x02); // response ARP
                    ft_memcpy(arpsend->sender_mac, s->sll_addr, ETH_ALEN);
                    ft_memcpy(arpsend->sender_ip, target->sin_addr.s_addr, 4);
                    ft_memcpy(arpsend->target_mac, arp->sender_mac, ETH_ALEN);
                    ft_memcpy(arpsend->target_ip, arp->sender_ip, 4);


                }
            }
        }
    }
        // int size = sendto(sockfd, "message received", 17, 0, (struct sockaddr *)&target, sizeof(target));
        // if(size < 0)
        // {
        //     printf("big problem\n");
        // }
        // else
        // {
        //     printf("Reply sent to %s:%d\n", inet_ntoa(target.sin_addr), ntohs(target.sin_port));
        // }
    close(sockfd);
    freeifaddrs(ifaddr);
    return(0);
}

// int send_arp(int fd, int ifindex, const unsigned char *src_mac, uint32_t src_ip, uint32_t dst_ip)
// {
//     int err = -1;
//     unsigned char buffer[BUF_SIZE];
//     memset(buffer, 0, sizeof(buffer));

//     struct sockaddr_ll socket_address;
//     socket_address.sll_family = AF_PACKET;
//     socket_address.sll_protocol = htons(ETH_P_ARP);
//     socket_address.sll_ifindex = ifindex;
//     socket_address.sll_hatype = htons(ARPHRD_ETHER);
//     socket_address.sll_pkttype = (PACKET_BROADCAST);
//     socket_address.sll_halen = MAC_LENGTH;
//     socket_address.sll_addr[6] = 0x00;
//     socket_address.sll_addr[7] = 0x00;

//     struct ethhdr *send_req = (struct ethhdr *) buffer;
//     struct arp_header *arp_req = (struct arp_header *) (buffer + ETH2_HEADER_LEN);
//     int index;
//     ssize_t ret, length = 0;

//     //Broadcast
//     memset(send_req->h_dest, 0xff, MAC_LENGTH);

//     //Target MAC zero
//     memset(arp_req->target_mac, 0x00, MAC_LENGTH);

//     //Set source mac to our MAC address
//     memcpy(send_req->h_source, src_mac, MAC_LENGTH);
//     memcpy(arp_req->sender_mac, src_mac, MAC_LENGTH);
//     memcpy(socket_address.sll_addr, src_mac, MAC_LENGTH);

//     /* Setting protocol of the packet */
//     send_req->h_proto = htons(ETH_P_ARP);

//     /* Creating ARP request */
//     arp_req->hardware_type = htons(HW_TYPE);
//     arp_req->protocol_type = htons(ETH_P_IP);
//     arp_req->hardware_len = MAC_LENGTH;
//     arp_req->protocol_len = IPV4_LENGTH;
//     arp_req->opcode = htons(ARP_REQUEST);

//     debug("Copy IP address to arp_req");
//     memcpy(arp_req->sender_ip, &src_ip, sizeof(uint32_t));
//     memcpy(arp_req->target_ip, &dst_ip, sizeof(uint32_t));

//     ret = sendto(fd, buffer, 42, 0, (struct sockaddr *) &socket_address, sizeof(socket_address));
//     if (ret == -1) {
//         perror("sendto():");
//         goto out;
//     }
//     err = 0;
// out:
//     return err;
// }