/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpoyet <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 10:07:51 by bpoyet            #+#    #+#             */
/*   Updated: 2023/11/17 15:53:48 by bpoyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t size)
{
	char		*ptrdest;
	const char	*ptrsrc;

	ptrdest = (char *)dest;
	ptrsrc = (char *)src;
	if (!dest && !src)
		return (NULL);
	while (size > 0)
	{
		*ptrdest++ = *ptrsrc++;
		size--;
	}
	return (dest);
}
