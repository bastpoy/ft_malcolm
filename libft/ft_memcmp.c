/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpoyet <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 13:54:52 by bpoyet            #+#    #+#             */
/*   Updated: 2023/11/14 13:54:55 by bpoyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *ptr1, const void *ptr2, size_t size)
{
	unsigned char	*cpptr1;
	unsigned char	*cpptr2;

	cpptr1 = (unsigned char *)ptr1;
	cpptr2 = (unsigned char *)ptr2;
	if (size == 0)
		return (0);
	while (size > 0)
	{
		if (*cpptr1 != *cpptr2)
			return (*cpptr1 - *cpptr2);
		cpptr1++;
		cpptr2++;
		size--;
	}
	return (0);
}
