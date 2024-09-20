/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malcolm.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bastpoy <bastpoy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 18:06:16 by bpoyet            #+#    #+#             */
/*   Updated: 2024/09/20 21:33:26 by bastpoy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 3000

int main(void) 
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
    int numbytes;
    char buffer[1024];
    
    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    myaddr.sin_port = htons(PORT);
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = INADDR_ANY;
    memset(&(myaddr.sin_zero), '\0', 8);

    printf("my address is %s\n",  inet_ntoa(myaddr.sin_addr));

    if(bind(sockfd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr)) == -1)
    {
        perror("Bind error:");
        return(errno);
    }
    printf("Bind ok!\n");
    if(listen(sockfd, 10) == -1)
    {
        perror("Listen error: ");
        return(errno);
    }
    printf("Listen ok!\n");
    while(1)
    {
        sin_size = sizeof(struct sockaddr_in);
        if((new_fd = accept(sockfd, (struct sockaddr *)&theiraddr, &sin_size)) == -1)
        {
            perror("error accept:");
            return(1);
        }
        printf("the address is: %s\n", inet_ntoa(theiraddr.sin_addr));
        printf("Connection from: %s\n", inet_ntoa(theiraddr.sin_addr));

        numbytes = recv(new_fd, buffer, sizeof(buffer)-1, 0);
        if(numbytes == -1)
        {
            perror("recv error:");
            return(1);
        }

        buffer[numbytes] = '\0';  // Null-terminate the received data
        printf("Received: %s\n", buffer);
        close(new_fd);
    }
    close(sockfd);
}

