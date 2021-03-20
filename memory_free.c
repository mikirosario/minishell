/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_free.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/05 18:23:53 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/20 18:32:55 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Frees all memory reserved for a linked list, without attempting to free the
** memory taken up by its content. ;)
*/

t_list	*ft_lstfree(t_list *lst)
{
	t_list	*next;

	if (lst)
	{
		next = lst;
		while (next)
		{
			lst = next;
			next = lst->next;
			free(lst);
		}
	}
	return (NULL);
}

/*
** This function frees memory reserved to store dynamic cmdline information.
**
** Pointers used to check for reserved memory are set to NULL.
*/

void	clear_cmdline(t_micli *micli)
{
	if (micli->cmdline.cmd)
		micli->cmdline.cmd = ft_del(micli->cmdline.cmd);
	if (micli->cmdline.micli_argv)
		micli->cmdline.micli_argv = ft_del(micli->cmdline.micli_argv);
	if (micli->cmdline.arguments)
		ft_lstclear(&micli->cmdline.arguments, free);
	if (micli->token.var_lst)
		micli->token.var_lst = ft_lstfree(micli->token.var_lst);
	ft_bzero(&micli->cmdline, sizeof(t_cmdline));
	micli->cmdline.arguments = NULL;
}

/*
** This function should free any memory that needs to be freed before program
** termination.
*/

void	freeme(t_micli *micli)
{
	if (micli->cmd_result_str)
		micli->cmd_result_str = ft_del(micli->cmd_result_str);
	if (micli->buffer)
		micli->buffer = ft_del(micli->buffer);
	if (micli->cmdhist.hist)
		ft_free_split(micli->cmdhist.hist);
	clear_cmdline(micli);
	if (micli->tokdata.path_array)
		micli->tokdata.path_array = ft_free_split(micli->tokdata.path_array);
	if (micli->pipes.array)
		micli->pipes.array = ft_del(micli->pipes.array);
	if (micli->envp)
		micli->envp = ft_free_split(micli->envp);
}
