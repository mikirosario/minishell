/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_handling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 12:13:45 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/04 19:06:24 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function frees memory reserved to store dynamic token information.
**
** Pointers used to check for reserved memory are set to NULL.
*/

void	free_token(t_micli *micli)
{
	t_list *lst;

	//Check token struct for anything that needs to be freed and free as needed.
	lst = micli->token->arguments;
	if (micli->token->cmd)
		micli->token->cmd = ft_del(micli->token->cmd);
	if (micli->token->arguments)
		while (lst)
		{
			if (lst->content)
				lst->content = ft_del(lst->content);
			lst = lst->next;
		}
	if (micli->token->arg_array)
		micli->token->arg_array = ft_del(micli->token->arg_array);
	//	ft_lstiter(micli->token->arguments, free); //My lstiter leads to unallocated pointers being freed... :p
	ft_lstclear(&micli->token->arguments, free);
	micli->token->arguments = NULL;
	micli->token = NULL; //token struct memory is reserved locally by the tokenize function for the moment, so does not need to be freed, only the pointer nullified for validity of the check.
}

/*
** This function should free any memory that needs to be freed before program
** termination.
*/

void	freeme(t_micli *micli)
{
	if (micli->buffer)
		micli->buffer = ft_del(micli->buffer);
	if (micli->token)
		free_token(micli);
}

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