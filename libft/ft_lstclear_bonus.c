/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/16 15:24:57 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/08 00:53:53 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*tmp;
	t_list	*next;

	tmp = *lst;
	if (del && lst && *lst)
	{
		while (tmp)
		{
			next = tmp->next;
			ft_lstdelone(tmp, del);
			tmp = next;
		}
	}
	*lst = NULL;
}
