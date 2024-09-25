/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpoyet <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/16 18:40:58 by bpoyet            #+#    #+#             */
/*   Updated: 2023/11/20 12:35:31 by bpoyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <limits.h>

static long int	ft_chartoint(char *str, int sign)
{
	long long int	output;

	output = 0;
	while (*str >= '0' && *str <= '9')
	{
		output = output * 10 + (*str - '0');
		if (output < 0)
		{
			if (sign)
				return (0);
			else
				return (-1);
		}
		str++;
	}
	if (sign)
		return (-output);
	return (output);
}

static char	*space(char *str)
{
	while (*str == ' ' || *str == '\n' || *str == '\t' || *str == '\v'
		|| *str == '\f' || *str == '\r')
	{
		str++;
	}
	return (str);
}

int	ft_atoi(const char *str)
{
	int		sign;
	char	*ptr;

	ptr = (char *)str;
	sign = 0;
	if (*ptr == ' ' || *ptr == '\n' || *ptr == '\t' || *ptr == '\v'
		|| *ptr == '\f' || *ptr == '\r')
		ptr = space(ptr);
	if (*ptr == '-' || *ptr == '+')
	{
		if (*ptr == '-')
			sign = 1;
		ptr++;
	}
	if (*ptr >= '0' && *ptr <= '9')
		return (ft_chartoint(ptr, sign));
	return (0);
}

#include <stdio.h>

int main(void){
        printf("%d\n", ft_atoi("18446744073709551616"));
        printf("%d", atoi("18446744073709551616"));
}
