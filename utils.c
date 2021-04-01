/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/01 10:45:32 by miki              #+#    #+#             */
/*   Updated: 2021/04/01 10:58:23 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Just a putchar for termcaps to use as described in man putchar.
*/

int	pchr(int chr)
{
	if (write(STDOUT_FILENO, &chr, 1) == -1)
		return (EOF);
	return (chr);
}

/*
** For reasons I have yet to discover, Linux really seems get discombobulated
** when the OLDPWD environmental variable is undefined, even though this is what
** bash seems to do in Linux too... so clearly I am missing something about
** this. With OLDPWD undefined in my Linux home computer, some commands, like
** make, fail.
**
** This is a quick and dirty patch for that problem. If the program detects that
** it is running on Linux, it launches "linux_compatibility_mode", which
** basically just calls cd on '.' to set the current working directory as
** OLDPWD. This fixes any issues.
*/

void	linux_compatibility_mode(t_micli *micli)
{
	const char	*fake_argv[2];

	fake_argv[0] = "cd";
	fake_argv[1] = ".";
	ft_cd(fake_argv, micli);
}

/*
** This function is like strlen, but for null-terminated 16 bit strings.
*/

size_t	ft_strlen16(short *str)
{
	size_t	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}
