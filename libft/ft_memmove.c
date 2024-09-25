/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpoyet <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 11:02:54 by bpoyet            #+#    #+#             */
/*   Updated: 2023/11/17 15:54:09 by bpoyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t size)
{
	char		*ptrdest;
	const char	*ptrsrc;

	ptrdest = (char *)dest;
	ptrsrc = src;
	if (!dest && !src)
		return (0);
	if (ptrdest > ptrsrc)
	{
		while (size-- > 0)
			ptrdest[size] = ptrsrc[size];
	}
	else
	{
		while (size-- > 0)
			*ptrdest++ = *ptrsrc++;
	}
	return (dest);
}
