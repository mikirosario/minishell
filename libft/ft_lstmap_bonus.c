/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/16 17:52:00 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/25 03:05:38 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	norminette_made_me_do_it(t_list *cpy, void *(*f)(void *), \
void (*del)(void *))
{
	while (cpy)
	{
		cpy->content = f(cpy->content);
		if (!cpy->content)
			del(cpy->content);
		cpy = cpy->next;
	}
}

t_list	*ft_lstmap(t_list *l, void *(*f)(void *), void (*del)(void *))
{
	t_list	*cpy;
	t_list	*orig;
	t_list	*start;

	start = NULL;
	if (l)
	{
		start = ft_lstnew(l->content);
		orig = start;
		while (l->next)
		{
			l = l->next;
			cpy = ft_lstnew(l->content);
			orig->next = cpy;
			orig = cpy;
		}
		cpy = start;
		norminette_made_me_do_it(cpy, f, del);
	}
	return (start);
}
