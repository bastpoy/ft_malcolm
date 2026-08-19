#include "malcolm.h"

int getDestAddr(char *argv, struct sockaddr_in *addrDest, char *buffer){
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    char ipstr[INET_ADDRSTRLEN];
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    int ret = getaddrinfo(argv, NULL, &hints, &result);
    if(ret != 0){
        if(ret == -2){
            printf("ping: %s: Name or service not known\n", argv);
            freeaddrinfo(result);
        }
        else if (ret == -3){
            printf("ping: %s: Temporary failure in name resolution\n", argv);
            freeaddrinfo(result);
        }
        return 2;
    }
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_family == AF_INET) {
            struct sockaddr_in *temp = (struct sockaddr_in *)rp->ai_addr;
            memcpy(addrDest, (struct sockaddr_in *)rp->ai_addr, sizeof(struct sockaddr_in));
            inet_ntop(rp->ai_family, &(temp->sin_addr), ipstr, sizeof(ipstr));
            if (getnameinfo((struct sockaddr*)temp, sizeof(struct sockaddr), hbuf, sizeof(hbuf), sbuf,
                    sizeof(sbuf), NI_NAMEREQD) == 0){
                    memcpy(buffer, hbuf, sizeof(hbuf));
            }
            freeaddrinfo(result);
            return 0;
        }
    }
    freeaddrinfo(result);
    printf("ping: %s: Temporary failure in name resolution\n", argv);
    return 2;
}

int fill_addr(struct sockaddr_in *inAddress, char *addr, int argc, char *option, bool verbose_mode, struct Malcolm *malcolm)
{
    int rval = 0;
    char buffer[33];
    malcolm->verbose_mode = false;
    (void)verbose_mode;

    //fill port
    (*inAddress).sin_port = htons(PORT);
    //fill type of address
    (*inAddress).sin_family = AF_INET;
    //convert my input ip address ip to binary
    if(argc == 6 && !strcmp(option, "-d")){
        uint32_t number = strtoul(addr, NULL, 10);
        inAddress->sin_addr.s_addr = htonl(number);
        printf("IP address: %u\n", inAddress->sin_addr.s_addr);
        if(!(inet_ntop(AF_INET, &inAddress->sin_addr.s_addr, buffer, 33))){
            printf("problem converting address to host byte into char %s\n", strerror(errno));
            return(errno);
        }
        printf("buffer is %s\n", buffer);
        return(0);
    }
    else if(argc == 6 && !strcmp(option, "-h")){
        printf("in option hexadecimal\n");
        if(getDestAddr(addr, inAddress, NULL) != 0){
            return 1;
        }
    }
    else if(argc == 6 && !strcmp(option, "-v")){
        printf("je suis en verbose\n");
        malcolm->verbose_mode = true;
    }
    else
    {
        rval = inet_pton(AF_INET, addr, &inAddress->sin_addr.s_addr);
        if(rval == -1)
        {
            printf("Error translating target address: %s\n",strerror(errno));
            return(errno);
        }
    }
    printf("the value of the address is %u and the char is %s\n", ntohl(inAddress->sin_addr.s_addr), addr);
    ft_memset(&(inAddress->sin_zero), '\0', 8);
    return(0);
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

// Helper function to convert a hex character to its numeric value
int hex_char_to_int(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;  // Invalid character
}

// Convert MAC address string to unsigned char array
int mac_string_to_bytes(const char *mac_str, unsigned char mac[6]) {
    int i = 0;
    int byte_index = 0;
    
    while (mac_str[i] != '\0' && byte_index < 6) {
        int high = hex_char_to_int(mac_str[i]);
        int low = hex_char_to_int(mac_str[i + 1]);
        
        if (high == -1 || low == -1)
            return -1;  // Invalid character
        
        mac[byte_index] = (high << 4) | low;
        byte_index++;
        
        i += 3;  // Skip 2 hex chars + 1 colon
    }
    
    return (byte_index == 6) ? 0 : -1;  // Success if exactly 6 bytes
}

bool verify_arguments(int argc, char *argv[], struct Malcolm *malcolm){
    int have_option = 0;
    //verify arguments
    if(argc != 5 && argc != 6)
    {
        printf("Usage: %s <source IP> <source MAC> <target IP> <target MAC>\n", argv[0]);
        return(false);
    }
    else if((argc == 6 && strcmp(argv[1], "-v")) && (argc == 6 && strcmp(argv[1], "-d")) && (argc == 6 && strcmp(argv[1], "-h"))){
        printf("Usage: %s <source IP> <source MAC> <target IP> <target MAC>\n", argv[0]);
        return(false);
    }

    if(argc == 6){
        have_option = 1;
    }

    if(!verify_mac((unsigned char *)argv[2 + have_option], (unsigned char *)argv[4 + have_option]))
    {
        printf("Invalid MAC address format.\n");
        return(false);
    }

    printf("verbose mode is %d\n", malcolm->verbose_mode);
    //fill the ip address and mac address from arguments
    if(fill_addr(&malcolm->srcaddr, argv[1 + have_option], argc, argv[1], malcolm->verbose_mode, malcolm) || 
    fill_addr(&malcolm->targetaddr, argv[3 + have_option], argc, argv[1], malcolm->verbose_mode, malcolm)){
        return(false);
    }
    printf("verbose mode is %d\n", malcolm->verbose_mode);
    if(mac_string_to_bytes(argv[2 + have_option], malcolm->sourceMac) != 0) {
        printf("Invalid source MAC address format.\n");
        return false;
    }
    if(mac_string_to_bytes(argv[4 + have_option], malcolm->targetMac) != 0) {
        printf("Invalid target MAC address format.\n");
        return false;
    }
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

unsigned char *create_arp_response(struct Malcolm malcolm){
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
    ft_memcpy(ethsend->h_dest, malcolm.targetMac, ETH_ALEN); // MAC dest which is the mac src of the previous request
    ft_memcpy(ethsend->h_source, malcolm.sourceMac, ETH_ALEN); // MAC source which is my MAC address

    //arp header
    arpsend->hardware_type = htons(1); // ethernet
    arpsend->protocol_type = htons(ETH_P_IP);
    arpsend->hardware_len = 6; // mac length
    arpsend->protocol_len = 4; // ip length
    arpsend->opcode = htons(0x02); // response ARP

    ft_memcpy(arpsend->sender_mac, malcolm.sourceMac, ETH_ALEN); // fill with my MAC
    ft_memcpy(arpsend->sender_ip, &malcolm.srcaddr.sin_addr.s_addr, 4); // fill with initial target IP
    ft_memcpy(arpsend->target_mac, malcolm.targetMac, ETH_ALEN); // SRC MAC
    ft_memcpy(arpsend->target_ip, &malcolm.targetaddr.sin_addr.s_addr, 4); // SRC IP
    
    printf("Sender MAC: ");
    print_mac(arpsend->sender_mac);
    printf("target MAC: ");
    print_mac(arpsend->target_mac);
    
    return buffersend;
}