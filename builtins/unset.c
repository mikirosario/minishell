/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 17:09:30 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/06 20:51:18 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_unset(char **argv, t_micli *micli)
{
	size_t	name_len;
	char	*store;
	int		i;
	char	**temp;

	i = 0;
	if (argv[1] != 0)
	{
		name_len = ft_strlen(argv[1]);
		store = find_var(argv[1], name_len, &micli->envp[1]);
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
	}
	return (0);
}
