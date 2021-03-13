/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 17:09:30 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/13 14:31:09 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_unset(char **argv, t_micli *micli)
{
	size_t	name_len;
	char	*store;
	char	**old_envp;
	char	**temp;
	size_t		z;
	size_t		i;

	i = 0;
	z = 1;
	while (argv[z] != NULL)
	{
		name_len = ft_strlen(argv[z]);
		store = find_var(argv[z], name_len, micli->envp);
		if (store != NULL)
		{
			i = 0;
			while (micli->envp[i])
				i++;
			old_envp = micli->envp;
			micli->envp = clean_calloc(i, sizeof(char *), micli); //need i pointers, not i-1, because of NULL termination.	
			i = 0;
			temp = old_envp;
			while (*temp)
			{
				if (*temp != store)
					micli->envp[i++] = *temp;
				temp++;
			}
			old_envp = ft_del(old_envp);
		}
		z++;
	}
	return (0);
}
