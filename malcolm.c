#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>

#define PORT 219

int fill_addr(struct sockaddr_in *targetaddr, char *addr)
{
    int rval = 0;

    (*targetaddr).sin_port = htons(PORT);
    (*targetaddr).sin_family = AF_INET;
    if(!(rval = inet_pton(AF_INET, addr, &targetaddr->sin_addr.s_addr)))
    {
        printf("invalide address: %s\n", addr);
        return(1);
    }
    else if(rval == -1)
    {
        printf("Error translating target address: %s\n",strerror(errno));
        return(errno);
    }
    memset(&(targetaddr->sin_zero), '\0', 8);
    return(0);
}

int main(int argc, char *argv[]) 
{
    (void) argc;
    (void) argv;
    struct sockaddr_in rec;
    struct sockaddr_in *target;
    struct ifaddrs *ifaddr, *ifa ;
    int sockfd;
    int broadcast = 1;
    char buffer[1024];
    // socklen_t sin_size ;
    
    // sin_size = sizeof(struct sockaddr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // fill_addr(&targetaddr, argv[1]);
    // fill_addr(&srcaddr, argv[2]);

    if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == -1)
    {
        printf("error setting brodcast address: %s", strerror(errno));
        return(errno);
    }
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt(SO_REUSEADDR)");
        close(sockfd);
        return 1;
    }

    memset(&rec, 0, sizeof(rec));
    rec.sin_family = AF_INET;
    rec.sin_port = htons(PORT);
    rec.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&rec, sizeof(rec)) == -1) {
        perror("bind");
        close(sockfd);
        return (errno);
    }

    if(getifaddrs(&ifaddr) == -1)
    {
        printf("error on getting addr: %s\n", strerror(errno));
        return(errno);
    }
    for(ifa = ifaddr; ifa; ifa = ifa->ifa_next)
    {
        if(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET)
        {
            target = (struct sockaddr_in*)ifa->ifa_addr;
            inet_ntop(AF_INET, &target->sin_addr, buffer, 1024);
            printf("Interface : %s \t Address: %s \t MAC: %s\n", ifa->ifa_name, buffer, ifa->ifa_netmask->sa_data);
        }
        if (ifa->ifa_addr->sa_family == AF_PACKET && ifa->ifa_data != NULL) {
            struct sockaddr_ll *s = (struct sockaddr_ll *)ifa->ifa_addr;
            printf("Interface : %s \t MAC: ", ifa->ifa_name);
            for (int i = 0; i < s->sll_halen; i++) {
                printf("%02x%s", (unsigned char)s->sll_addr[i], (i + 1 != s->sll_halen) ? ":" : "");
            }
            printf("\n");
        }
    }
    freeifaddrs(ifaddr);
    // printf("listening...\n");
    // while(1)
    // {
    //     int len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&target, &sin_size);
    //     if(len == -1)
    //     {
    //         printf("Error recvfrom: %s\n", strerror(errno));
    //         return(errno);
    //     }
    //     else
    //     {
    //         buffer[len] = '\0';
    //         printf("receiving messages: %s\n", buffer);
    //     }
    //     int size = sendto(sockfd, "message received", 17, 0, (struct sockaddr *)&target, sizeof(target));
    //     if(size < 0)
    //     {
    //         printf("big problem\n");
    //     }
    //     else
    //     {
    //         printf("Reply sent to %s:%d\n", inet_ntoa(target.sin_addr), ntohs(target.sin_port));
    //     }
    // }
    close(sockfd);
    return(0);
}

