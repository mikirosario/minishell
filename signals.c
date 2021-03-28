/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/28 19:30:53 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/28 11:45:08 by miki             ###   ########.fr       */
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
	write(STDOUT_FILENO, "Quit\n", 5);
	signal(SIGQUIT, sigquit);
}

void	sigrun(int signum)
{
	if (signum == SIGINT)
	{
		write (STDOUT_FILENO, "🚀 ", 5);
		signal(SIGINT, sigrun);
	}
	else if (signum == SIGQUIT)
	{
		write(STDOUT_FILENO, "\n\x1b[D\x1b[C", 7);
		signal(SIGQUIT, sigrun);
	}
}
