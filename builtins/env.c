/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 17:09:59 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/28 03:30:02 by miki             ###   ########.fr       */
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

int	ft_env(char **envp, const char **argv)
{
	int		i;
	char	*store;

	store = *envp;
	i = 0;
	if (argv[1])
	{
		printf("env: %s: No such file or directory\n", argv[1]);
		return (127);
	}
	while (store && envp != NULL)
	{
		if (ft_strchr(store, '='))
			print_env(store);
		store = *(envp + ++i);
	}
	return (0);
}
