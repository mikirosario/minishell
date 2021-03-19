/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/28 19:30:53 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/19 04:01:01 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	waiting(int signum)
{
	(void)signum;
	signal(SIGINT, waiting);
}

void	sigrun(int signum)
{
	if (signum == SIGINT)
	{
		write(1, "\033[2D\033[J\n🚀 ", ft_strlen("\033[2D\033[J\n🚀 "));
		signal(SIGINT, sigrun);
	}
	else if (signum == SIGQUIT)
	{
		write(1, "\033[2D\033[J", ft_strlen("\033[2D\033[J"));
		signal(SIGQUIT, sigrun);
	}
}
