/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_reservation.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 12:13:45 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/26 09:14:23 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function is a wrapper around ft_memdup that sets the proper error flag
** and aborts the program in case of lack of memory. Errno 28 is for lack of
** memory. I hardcode it as ft_split does not set it.
*/

void	*clean_ft_memdup(void const *mem, size_t memsize, t_micli *micli)
{
	void	*ptr;

	ptr = ft_memdup(mem, memsize);
	if (!ptr)
	{
		micli->syserror = 28;
		exit_failure(micli);
	}
	return (ptr);
}



/*
** This function is a wrapper around ft_strdup that sets the proper error flag
** and aborts the program in case of lack of memory. Errno 28 is for lack of
** memory. I hardcode it as ft_split does not set it.
*/

char	*clean_ft_strdup(char const *str, t_micli *micli)
{
	char	*ptr;

	if (!(ptr = ft_strdup(str)))
	{
		micli->syserror = 28;
		exit_failure(micli);
	}
	return (ptr);
}

/*
** This function is a wrapper around ft_strjoin that sets the proper error flag
** and aborts the program in case of lack of memory. Errno 28 is for lack of
** memory. I hardcode it as ft_split does not set it.
*/

char	*clean_ft_strjoin(char const *s1, char const *s2, t_micli *micli)
{
	char	*ptr;

	if (!(ptr = ft_strjoin(s1, s2)))
	{
		micli->syserror = 28;
		exit_failure(micli);
	}
	return (ptr);
}

/*
** This function is a wrapper around ft_split that sets the proper error flag
** and aborts the program in case of lack of memory. Errno 28 is for lack of
** memory. I hardcode it as ft_split does not set it.
*/

char	**clean_ft_split(const char *s, char c, t_micli *micli)
{
	char **ptr;

	if (!s)
		return (NULL);
	if (!(ptr = ft_split(s, c)))
	{
		micli->syserror = 28;
		exit_failure(micli);
	}
	return (ptr);
}

/*
** This function reallocates old_size bytes of the memory pointed to by ptr to a
** new memory block of the size defined by new_size, freeing the old memory
** block.
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
**
** Remember the size of an array is not the same as the number of elements in
** it. An array of four integers is 4 * sizeof(int) = 16 bytes.
*/

void	*clean_realloc(void *ptr, size_t new_size, size_t old_size, t_micli *micli)
{
	void *tmp;

	tmp = ptr;
	if (!ptr)
		return (NULL);
	ptr = clean_calloc(new_size, 1, micli);
	if (!ptr)
		micli->syserror = errno;
	else
		ft_memcpy(ptr, tmp, old_size);
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
