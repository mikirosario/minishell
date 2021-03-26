/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/23 01:05:00 by miki              #+#    #+#             */
/*   Updated: 2021/03/25 03:07:05 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** Duplicates any block of memory and returns a pointer to the duplicate. You
** MUST specify the size of the memory to be duplicated. Returns NULL if the
** allocation fails.
*/

void	*ft_memdup(void const *mem, size_t memsize)
{
	void	*dup;

	dup = ft_calloc(memsize, 1);
	if (dup != NULL)
		ft_memcpy(dup, mem, memsize);
	return (dup);
}
