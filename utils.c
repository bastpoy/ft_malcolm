#include "malcolm.h"

int fill_addr(struct sockaddr_in *inAddress, char *addr)
{
    int rval = 0;
    char buffer[INET_ADDRSTRLEN];

    //fill port
    (*inAddress).sin_port = htons(PORT);
    //fill type of address
    (*inAddress).sin_family = AF_INET;
    //convert my input ip address ip to binary 
    if(!(rval = inet_pton(AF_INET, addr, &inAddress->sin_addr.s_addr)))
    {
        printf("invalide address: %s\n", addr);
        return(1);
    }
    else if(rval == -1)
    {
        printf("Error translating target address: %s\n",strerror(errno));
        return(errno);
    }
    //convert the initial address to print it
    inet_ntop(AF_INET, &inAddress->sin_addr, buffer, sizeof(buffer));
    printf("Addresse source %s\n", buffer);

    ft_memset(&(inAddress->sin_zero), '\0', 8);
    return(0);
}

void print_uchar(unsigned char *array)
{
    for(int i = 0; i < 4; i++)
    {
        if(i != 3)
            printf("%d-", array[i]);
        else
            printf("%d", array[i]);
    }
    // printf("i vaut %d", i);
    printf("\n");
}

void print_mac(unsigned char *mac) 
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
