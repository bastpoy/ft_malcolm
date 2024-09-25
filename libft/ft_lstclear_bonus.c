/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpoyet <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 23:24:44 by bpoyet            #+#    #+#             */
/*   Updated: 2023/11/17 15:49:19 by bpoyet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*buffer;

	if (!del)
		return ;
	if (lst)
	{
		while (*lst)
		{
			buffer = (*lst)->next;
			del((*lst)->content);
			free(*lst);
			*lst = buffer;
		}
	}
}
