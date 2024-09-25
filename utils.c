#include "malcolm.h"

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
    ft_memset(&(targetaddr->sin_zero), '\0', 8);
    return(0);
}