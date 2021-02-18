/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/18 12:14:45 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/18 16:48:49 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Count number of pipes in a line.
*/

size_t	pipe_count(const char *line, t_micli *micli)
{
	unsigned char	quote_flag;
	unsigned char	escape_flag;
	size_t			pipe_count;

	(void)micli;
	quote_flag = 0;
	pipe_count = 0;
	line = ft_skipspaces(line);
	while (*line)
	{
		escape_flag = 0;														//Status Conditions
		if (*line == '\\' && quote_flag != 2 && !escape_flag) 					//If escape is found and we aren't between single quotes and the escape hasn't itself been escaped... Note that single quotes prevent escaping.
		{
			escape_flag = 1;													//Escape flag is set.
			line++;																//Line is incremented by one to examine escaped character.
		}
		if ((*line == '"' && !escape_flag && (!quote_flag || quote_flag == 1)) 	//If double quotes are found and the double quotes have not been escaped and we are either not between quotes or we are between double quotes...
		|| (*line == '\'' && !escape_flag && (!quote_flag || quote_flag == 2)))	//OR if single quote is found and the single quote has not been escaped and we are either not between quotes or we are between single quotes.
			quote_flag = toggle_quote_flag(*line, quote_flag);					//Toggle the quote flag appropriately. Quote flag will toggle when: no quotes and quotes found or quotes and unescaped quotes of same type found.
		if (*line == '|' && !quote_flag && !escape_flag)						//If pipe is found and we aren't between any quotes and the pipe or semicolon hasn't been escaped...
			pipe_count++;														//Count the pipe.
		line++;																	//Go to next char.
	}
	return (pipe_count);
}