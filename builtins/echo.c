/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 17:08:53 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/28 03:23:41 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_echo(const char **argv, t_micli *micli)
{
	int		i;
	char	*n;
	int		len;

	n = "-n";
	len = 2;
	i = 1;
	micli->builtins.argflag = 0;
	if (!argv[i])
	{
		printf("\n");
		return (0);
	}
	i = 1;
	while (!(ft_memcmp(argv[i], n, len)))
	{
		micli->builtins.argflag = 1;
		if (argv[i + 1] == NULL)
			return (0);
		i++;
	}
	echo_helper(i, argv, micli);
	return (0);
}

void	echo_helper(int i, const char **argv, t_micli *micli)
{
	while (argv[i] != 0)
	{
		if (argv[i + 1])
			printf("%s ", argv[i]);
		else
			printf("%s", argv[i]);
		i++;
	}
	if (micli->builtins.argflag != 1)
		printf("\n");
}
