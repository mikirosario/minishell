/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_handling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 12:13:45 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/12 00:18:58 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function reallocates the memory of the string pointed to by ptr to a
** new memory block of the size defined by size, freeing the old memory block.
** 
** If a null pointer is passed, a null pointer will be returned and nothing
** will be freed. Freeing a null pointer results in no operation being
** performed, so it's fine.
**
** If the reallocation fails, the old memory block is destroyed and a null
** pointer is returned. Errno at failure is saved to a variable in the micli
** struct for use in subsequent error handling.
**
** This function uses memcpy, which is not safe if dst and src overlap.
**
** Behaviour is undefined if size is less than the memory to be reallocated.
** Probably means segfault. So just don't do that. ;)
*/

char	*ft_realloc(char *ptr, size_t size, t_micli *micli)
{
	char *tmp;

	tmp = ptr;
	if (!ptr || !(ptr = malloc(sizeof(char) * size)))
		micli->syserror = errno;
	else
		ft_memcpy(ptr, tmp, size);
	//printf("Bufsize: %zu\n", size); (Debug code)
	tmp = ft_del(tmp);
	return (ptr);
}

/*
** This function wraps ft_calloc in a security layer that sets the proper error
** flag and aborts the program in case of lack of memory.
*/

void	*clean_calloc(size_t count, size_t size, t_micli *micli)
{
	void	*ptr;
	if (!(ptr = ft_calloc(count, size)))
	{
		micli->syserror = errno;
		exit_failure(micli);
	}
	return (ptr);
}