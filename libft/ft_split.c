/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpoyet <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 16:06:49 by bpoyet            #+#    #+#             */
/*   Updated: 2023/11/17 19:23:03 by bpoyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_countsep(char const *s, char c)
{
	size_t	count;
	int		state;

	state = 0;
	count = 0;
	while (*s)
	{
		if (*s != c && state == 0)
		{
			count++;
			state = 1;
		}
		if (*s == c && state == 1)
			state = 0;
		s++;
	}
	return (count);
}

static int	ft_addword(char **str, const char *s, size_t *i, size_t *j)
{
	size_t		l;
	size_t		k;
	const char	*ptr;

	k = *j;
	ptr = s - *i;
	l = 0;
	str[k] = (char *)malloc((*i + 1) * sizeof(char));
	if (!str[k])
		return (0);
	while (l < *i)
	{
		str[k][l] = ptr[l];
		l++;
	}
	str[k][l] = 0;
	*i = 0;
	*j = *j + 1;
	return (1);
}

static void	*ft_testmalloc(char **str, size_t j)
{
	size_t	i;

	i = 0;
	while (i < j)
	{
		free(str[i]);
		i++;
	}
	free(str[i]);
	free(str);
	return (0);
}

static void	ft_init(size_t *i, size_t *j)
{
	*i = 0;
	*j = 0;
}

char	**ft_split(char const *s, char c)
{
	char	**str;
	size_t	i;
	size_t	j;

	ft_init(&i, &j);
	str = malloc((ft_countsep(s, c) + 1) * sizeof(char *));
	if (!str)
		return (0);
	while (*s)
	{
		while (*s == c && *s)
			s++;
		while (*s != c && *s)
		{
			s++;
			i++;
			if ((*s == c || *s == '\0'))
			{
				if (!ft_addword(str, s, &i, &j))
					return (ft_testmalloc(str, j));
			}
		}
	}
	str[j] = NULL;
	return (str);
}
