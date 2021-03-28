/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/28 19:30:53 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/27 21:14:42 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	waiting(int signum)
{
	(void)signum;
	signal(SIGINT, waiting);
}

void	sigquit(int signum)
{
	(void)signum;
	write(STDOUT_FILENO, "Quit", 4);
	signal(SIGQUIT, sigquit);
}

void	sigrun(int signum)
{
	if (signum == SIGINT)
	{
		write (STDOUT_FILENO, "\n🚀 ", 6);
		signal(SIGINT, sigrun);
	}
	else if (signum == SIGQUIT)
	{
		write(STDOUT_FILENO, "\x1b[D\x1b[C", 6);
		signal(SIGQUIT, sigrun);
	}
}
