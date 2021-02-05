/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_free.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/05 18:23:53 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/05 18:32:52 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Frees all memory reserved for a character pointer array with ft_split, first
** freeing the lines pointed to by each pointer, then freeing the pointer array
** itself.
*/

char	**free_split(char **split)
{
	int	i;

	i = 0;
	while (split[i])
	{
		if (split)
		split[i] = ft_del(split[i]);
		i++;
	}
	free(split);
	return (NULL);
}

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
	if (micli->token->micli_argv)
		micli->token->micli_argv = ft_del(micli->token->micli_argv);
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