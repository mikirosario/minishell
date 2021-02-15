/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   iamerror.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/08 19:25:04 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/15 17:59:41 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	print_error(char *error_message, char *error_location)
{
	ft_printf("%s '%.2s'\n", error_message, error_location); //Print two chars here but for echo ;| ls, for example, should only print '|'
	return (0);
}

/*
** This function parses the whole line checking for the following syntax errors:
**
** 1. First character means end of command line (';' or '|').
** 2. Between a character meaning end of command line and the following command
** line there are more characters meaning end of command line, i.e.: ls;;ls or 
** ls;|ls.
**
** Characters enclosed by quotes are not considered to mean end of command line.
** To keep track of this we use a quote_flag that is toggled when quotes are
** found. This is a simpler check than the token parser proper as any open
** quotes will designify the end of command line characters.
**
** If a syntax error is found we print an error message and return 0. Otherwise
** we return 1.
**
** NOTE: Treating '||' as a syntax error is not exactly bash behaviour, as bash
** implements '||' as meaning execute next command only if preceding command
** returned false, but the minishell subject does not require us to implement
** this feature, so I just treat it as a syntax error.
*/

int	syntax_check(char *line)
{
	unsigned char	quote_flag;

	quote_flag = 0;
	line = ft_skipspaces(line);
	if (*line != ';' && *line != '|')
		while (*line)
		{
			if (!quote_flag && (*line == ';' || *line == '|'))
			{
				line = ft_skipspaces(++line);
				if (*line == ';' || *line == '|')
					return(print_error(SYN_ERROR, line));
			}
			else
			{
				quote_flag = toggle_quote_flag(*line, quote_flag);
				line++;
			}
		}
	else
		return(print_error(SYN_ERROR, line));
	return (1);
}