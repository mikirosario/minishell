/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/25 22:09:22 by miki              #+#    #+#             */
/*   Updated: 2021/03/26 02:47:50 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** This function reallocates old_size bytes of the memory pointed to by ptr to a
** new memory block of the size defined by new_size, freeing the old memory
** block.
**
** If a null pointer is passed, or if new_size is 0, or if new_size is less than
** old_size, a null pointer will be returned and the old memory will not be
** freed.
**
** If the memory allocation fails, a null pointer will be returned and the old
** memory will not be freed.
**
** If old_size is 0, this function will just act the same as ft_calloc and
** reserve new_size bytes of memory.
**
** If old_size > 0, then this function will use memcpy, which is not safe if dst
** and src overlap, to copy the data from ptr to the corresponding bytes of the
** new memory. The old memory will then be freed.
**
** Remember the size of an array is not the same as the number of elements in
** it. An array of four integers is 4 * sizeof(int) = 16 bytes. You need to
** specify the SIZE IN BYTES of your old and new arrays, NOT the number of
** elements.
**
** If memory is successfully allocated, a pointer to the new memory will be
** returned.
*/

void	*ft_realloc(void *ptr, size_t new_size, size_t old_size)
{
	void	*tmp;

	if (!ptr || !new_size || new_size < old_size)
		return (NULL);
	ptr = ft_calloc(new_size, 1);
	if (!ptr)
		return (NULL);
	if (old_size)
	{
		tmp = ptr;
		ft_memcpy(ptr, tmp, old_size);
		tmp = ft_del(tmp);
	}
	return (ptr);
}
