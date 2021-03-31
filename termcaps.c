/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   termcaps.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/30 20:42:46 by miki              #+#    #+#             */
/*   Updated: 2021/03/31 06:22:19 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>

int	pchr(int chr)
{
	return(write(STDOUT_FILENO, &chr, 1));
}

void	termcaps(t_micli *micli, t_cmdhist *cmdhist)
{

	int co;

	(void)cmdhist;
	(void)micli;
	co = tgetnum("co");
	printf("\n%d\n", co);
	// // tputs(up, 1, pchr);
	// //free(up);
}