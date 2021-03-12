/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 17:09:30 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/12 21:54:17 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_unset(char **argv, t_micli *micli)
{
	size_t	name_len;
	char	*store;
	int		i;
	char	**temp;
	int		z;

	i = 0;
	z = 1;
	while (argv[z] != 0)
	{
		name_len = ft_strlen(argv[z]);
		store = find_var(argv[z], name_len, micli->envp);
		if (store != 0)
		{
			ft_memset(store, 0, sizeof(store));
			temp = micli->envp;
			micli->envp = clean_calloc(i, sizeof(char *), micli);
			i = 0;
			while (*temp)
			{
				if (*temp != store)
					micli->envp[i++] = *temp;
				temp++;
			}
		}
		z++;
	}
	return (0);
}
