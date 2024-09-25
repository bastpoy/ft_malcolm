/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpoyet <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 17:24:47 by bpoyet            #+#    #+#             */
/*   Updated: 2023/11/17 19:14:36 by bpoyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *str, int c, size_t size)
{
	unsigned char	*ptr;

	ptr = (unsigned char *)str;
	while (size > 0)
	{
		if (*ptr == (unsigned char)c)
			return ((void *)str);
		ptr++;
		str++;
		size--;
	}
	return (0);
}
