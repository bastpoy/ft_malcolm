/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpoyet <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 00:24:49 by bpoyet            #+#    #+#             */
/*   Updated: 2023/11/17 19:14:12 by bpoyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	writenbr(char c, int fd)
{
	write(fd, &c, 1);
}

void	ft_putnbr_fd(int n, int fd)
{
	unsigned int	absolut;

	if (n < 0)
	{
		absolut = -n;
		writenbr('-', fd);
	}
	else
		absolut = n;
	if (absolut > 9)
	{
		ft_putnbr_fd(absolut / 10, fd);
	}
	writenbr(absolut % 10 + '0', fd);
}
