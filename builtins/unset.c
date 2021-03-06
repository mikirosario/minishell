/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 17:09:30 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/28 03:32:17 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_unset(char **argv, t_micli *micli)
{
	size_t	name_len;
	char	*store;
	size_t	z;

	z = 1;
	while (argv[z] != NULL)
	{
		name_len = ft_strlen(argv[z]);
		store = find_var(argv[z], name_len, micli->envp);
		if (store != NULL && (ft_strncmp(argv[z], "_=", 2) \
		 && ft_strncmp(argv[z], "_\0", 2)))
			unset_helper(store, micli);
		z++;
	}
	return (0);
}

void	unset_helper(char *store, t_micli *micli)
{
	size_t	i;
	char	**old_envp;
	char	**temp;

	i = 0;
	while (micli->envp[i])
		i++;
	old_envp = micli->envp;
	micli->envp = clean_calloc(i, sizeof(char *), micli);
	i = 0;
	temp = old_envp;
	while (*temp)
	{
		if (*temp != store)
			micli->envp[i++] = *temp;
		temp++;
	}
	store = ft_del(store);
	old_envp = ft_del(old_envp);
}
