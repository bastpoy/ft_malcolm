/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malcolm.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bastpoy <bastpoy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 18:06:16 by bpoyet            #+#    #+#             */
/*   Updated: 2024/09/19 17:02:17 by bastpoy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>

#define PORT 2020

int main(int argc, char *argv[]) 
{
    // declarer la structure
    // socket
    //bind
    //listen
    //accept
    struct sockaddr_in myaddr;
    struct sockaddr_in theiraddr;
    socklen_t sin_size ;

    int sockfd, new_fd;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    myaddr.sin_port = htons(PORT);
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = INADDR_ANY;
    memset(&(myaddr.sin_zero), '\0', 8);

    bind(sockfd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr));
    listen(sockfd, 10);
    
    while(1)
    {
        sin_size = sizeof(struct sockaddr_in);
        if(new_fd = accept(sockfd, (struct sockaddr *)&theiraddr, &sin_size) == -1)
        {
            perror("error accept:");
            return(1);
        }
        printf("the address is: %s", inet_ntoa(theiraddr.sin_addr));
    }
}

