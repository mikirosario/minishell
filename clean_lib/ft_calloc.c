/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/09 14:45:04 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/19 21:10:25 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <stdlib.h>

void	*ft_calloc(size_t count, size_t size)
{
	void	*ptr;

	if (!(ptr = malloc(count * size)))
		return (ptr);
	ft_bzero(ptr, count * size);
	return (ptr);
}
