/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flag_toggles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 12:30:37 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/04 12:34:06 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Function to toggle a bit between 0 and 1. Returns bit-set variable.
**
** 0 is the rightmost bit.
**
** If bit is set higher than 8, behaviour is undefined.
*/

unsigned char	ft_togglebit(unsigned char byte, unsigned char bit)
{
	unsigned char	mask;

	mask = (unsigned char)1 << bit;
	return (mask ^ byte);
}

/*
** Update the quotes flag if quotes are detected.
**
** The quote_flag is a two bit unsigned integer in the following format:
**
** single:double
**
** 00 (0) = All quotes closed.
** 01 (1) = Double quotes opened, single quotes closed.
** 10 (2) = Single quotes opened, double quotes closed.
** 11 (3) = Double quotes opened, single quotes opened.
*/

unsigned char	toggle_quote_flag(char quotes, char quote_flag)
{
	if (quotes == '"')
		quote_flag = ft_togglebit(quote_flag, 0);
	else if (quotes == '\'')
		quote_flag = ft_togglebit(quote_flag, 1);
	return(quote_flag);
}