/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 17:09:59 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/06 17:25:57 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_env(char *str)
{
	while (*str != 0)
	{
		if (*str != '"')
			write(1, str, 1);
		str++;
	}
	write(1, "\n", 1);
}

int	ft_env(char **envp)
{
	int		i;
	char	*store;

	store = *envp;
	i = 0;
	while (store && envp != NULL)
	{
		if (ft_strchr(store, '='))
			print_env(store);
		store = *(envp + ++i);
	}
	return (0);
}
