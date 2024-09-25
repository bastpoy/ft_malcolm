/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpoyet <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 12:37:26 by bpoyet            #+#    #+#             */
/*   Updated: 2023/11/19 18:02:57 by bpoyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	countnum(int n)
{
	size_t			i;
	unsigned int	nb;

	i = 0;
	if (n < 0)
	{
		nb = -n;
		i++;
	}
	else
		nb = n;
	while (nb > 9)
	{
		nb /= 10;
		i++;
	}
	return (i + 1);
}

static void	ft_putnbr(int nb, char *str, size_t *i)
{
	unsigned int	absolut;

	if (nb < 0)
	{
		absolut = -nb;
		str[*i] = '-';
		*i += 1;
	}
	else
		absolut = nb;
	if (absolut > 9)
	{
		ft_putnbr(absolut / 10, str, i);
	}
	str[*i] = absolut % 10 + '0';
	*i += 1;
}

char	*ft_itoa(int n)
{
	int		count;
	char	*str;
	size_t	i;

	i = 0;
	count = countnum(n);
	str = (char *)malloc((count + 1) * sizeof(char));
	if (!str)
		return (NULL);
	ft_putnbr(n, str, &i);
	str[count] = '\0';
	return (str);
}
