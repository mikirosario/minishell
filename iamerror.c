/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   iamerror.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/08 19:25:04 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/18 17:57:04 by mrosario         ###   ########.fr       */
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
**
** NOTE: Should base all future char analysis on this algorithm. Seems to be the
** most bash-like.
*/

int	syntax_check(char *line)
{
	unsigned char	quote_flag;
	unsigned char	escape_flag;

	quote_flag = 0;
	line = ft_skipspaces(line);
	if (*line != ';' && *line != '|')
		while (*line)
		{
			escape_flag = 0;														//Status Conditions
			if (*line == '\\' && quote_flag != 2 && !escape_flag) 					//If escape is found and we aren't between single quotes and the escape hasn't itself been escaped... Note that single quotes prevent escaping.
			{																		//If escape is found and quote_flag == 1 and *line == $ " \ FALTA ESTA CONDICIONALIDAD 
				escape_flag = 1;													//Escape flag is set.
				line++;																//Line is incremented by one to examine escaped character.
			}
			if ((*line == '"' && !escape_flag && (!quote_flag || quote_flag == 1)) 	//If double quotes are found and the double quotes have not been escaped and we are either not between quotes or we are between double quotes...
			|| (*line == '\'' && !escape_flag && (!quote_flag || quote_flag == 2)))	//OR if single quote is found and the single quote has not been escaped and we are either not between quotes or we are between single quotes.
				quote_flag = toggle_quote_flag(*line, quote_flag);					//Toggle the quote flag appropriately. Quote flag will toggle when: no quotes and quotes found or quotes and unescaped quotes of same type found.
			if ((*line == '|' || *line == ';') && !quote_flag && !escape_flag)		//If pipe or semicolon is found and we aren't between any quotes and the pipe or semicolon hasn't been escaped...
			{						
				line = ft_skipspaces(++line);										//Skip any subsequent spaces, as they have no effect on syntax...
				if (*line == '|' || *line == ';')									//If we find another consecutive pipe or semicolon... (i.e. echo ;   ; echo)
					return(print_error(SYN_ERROR, line));							//Print syntax error and return failure code.
			}
			line++;																	//Go to next char.
		}
		// while (*line)
		// {
		// 	escape_flag = 0;x
		// 	if ((!quote_flag || quote_flag == 1) && *line == '\\') //Backlash escapes '"', '\' and '$' in double quotes.
		// 		escape_flag = 1;
		// 	if (!quote_flag && !escape_flag && (*line == ';' || *line == '|'))
		// 	{
		// 		line = ft_skipspaces(++line);
		// 		if (*line == ';' || *line == '|')
		// 			return(print_error(SYN_ERROR, line));
		// 	}
		// 	else
		// 	{
		// 		if ((!escape_flag || quote_flag == 2) && ((*line == '"' && quote_flag != 1) || (*line == '\'' && quote_flag != 2))) //single quotes deactivate escape flag, so '' is a quoted backslash, NOT an open quote with escaped backslash, whereas "\" is an open quote with an escaped double quote that hasn't been closed. :p Char processing doesn't take this into account, so needs to be revised *sigh*
		// 											//Note for future self: quotes also deactivate each other. so "'\" is actually open double quotes and an escaped double quote. single quote don't open here. Alo not working like this in char checker. double sigh.
		// 			quote_flag = toggle_quote_flag(*line, quote_flag);
		// 		line++;
		// 	}
		// }
	else
		return(print_error(SYN_ERROR, line));										//If ';' or '|' is found as first character after skipping initial spaces, it's always a syntax error.
	return (1);																		//If no syntax errors are found, return success code.
}