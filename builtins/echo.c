/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 17:08:53 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/13 19:14:38 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int		ft_echo(const char **argv, t_micli *micli)
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
		ft_printf("\n");
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
		ft_printf(argv[i + 1] ? "%s " : "%s", argv[i]);
		i++;
	}
	if (micli->builtins.argflag != 1)
		ft_printf("\n");
}
