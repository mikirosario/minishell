/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_argv.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/30 19:25:50 by miki              #+#    #+#             */
/*   Updated: 2021/03/30 19:27:14 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function reserves memory for and copies the memory addresses to an array
** of pointers wherein array[0] points to the command name, array[end] points to
** NULL, and all pointers in between point to the successive command arguments.
**
** Memory is reserved for micli->tokdata.args + 2 pointers, that is, for all
** arguments plus the command plus the null pointer. This array can be passed to
** execve.
**
** The free_token function will free memory reserved for this array.
*/

char	**create_micli_argv(char *cmd, t_list *arglst, t_micli *micli)
{
	char	**argv;
	size_t	i;

	i = 0;
	argv = clean_calloc(micli->tokdata.args + 2, sizeof(char *), micli);
	while (i < micli->tokdata.args + 2)
	{
		if (!i)
			argv[i++] = cmd;
		else
		{
			if (arglst)
			{
				argv[i++] = arglst->content;
				arglst = arglst->next;
			}
			else
				argv[i++] = NULL;
		}
	}
	return (argv);
}
