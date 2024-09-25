/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpoyet <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 14:31:21 by bpoyet            #+#    #+#             */
/*   Updated: 2023/11/18 17:13:33 by bpoyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_trym_start(char const *s1, char const *set)
{
	size_t	i;

	i = 0;
	while (s1[i])
	{
		if (ft_strchr(set, s1[i]) == 0)
			break ;
		i++;
	}
	return (i);
}

static int	ft_trym_end(char const *s1, char const *set)
{
	size_t	i;

	i = ft_strlen((char *)s1);
	while (i > 0)
	{
		if (ft_strchr(set, s1[i - 1]) == 0)
			break ;
		i--;
	}
	return (i);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	indice_start;
	size_t	indice_end;
	char	*str;
	size_t	i;

	i = 0;
	indice_start = ft_trym_start(s1, set);
	indice_end = ft_trym_end(s1, set);
	if (indice_start > indice_end)
		return (ft_strdup(""));
	str = (char *)malloc((indice_end - indice_start + 1) * sizeof(char));
	if (!str)
		return (0);
	while (indice_start + i < indice_end)
	{
		str[i] = s1[indice_start + i];
		i++;
	}
	str[i] = 0;
	return (str);
}
