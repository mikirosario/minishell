/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/05 21:11:46 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/19 21:10:25 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	*ft_memchr(void const *s, int c, size_t n)
{
	char const	*ptr;

	ptr = s;
	if (n > 0)
	{
		while (n-- > 0)
		{
			if (*ptr++ == c)
				return ((void *)(ptr - 1));
		}
	}
	return (0);
}
