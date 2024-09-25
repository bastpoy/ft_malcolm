/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpoyet <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/14 18:27:24 by bpoyet            #+#    #+#             */
/*   Updated: 2023/11/17 15:57:57 by bpoyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, char *src, size_t size)
{
	unsigned int	i;
	unsigned int	len_src;
	unsigned int	len_dest;

	i = 0;
	len_dest = ft_strlen(dest);
	len_src = ft_strlen(src);
	if (len_dest > size)
	{
		return (size + len_src);
	}
	if (size < 1)
	{
		return (len_src + size);
	}
	while (src[i] && (i + 1 + len_dest) < size)
	{
		dest[i + len_dest] = src[i];
		i++;
	}
	dest[i + len_dest] = '\0';
	return (len_dest + len_src);
}
