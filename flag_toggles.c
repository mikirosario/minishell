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
** Update the pipe flag if pipes are detected.
**
** The pipe_flag is a two bit unsigned integer in the following format:
**
** 								PIPE_FLAG
** BINARY DECIMAL
**
** 	00 		0 		= No pipe.
** 	01 		1		= first pipe, read from stdin, write to pipe
** 	10 		2 		= last pipe, read from pipe, write to stdout
** 	11 		3 		= middle pipe, read from and write to pipe
*/

unsigned char	toggle_pipe_flag(char pipe, unsigned char pipe_flag)
{
	if (pipe == '|') //if pipe is found and...
	{
		if (!ft_isbitset(pipe_flag, 0)) //If pipe_flag was zero or two, then there was 
										//no preceding piped command or the preceding
										//command was read-only, so set to 1 for write-only.
			pipe_flag = 1; 	//If this is our first cmd: echo 2*2 | or if last command was the read-end of a pipe: echo 2*2 | bc; echo 2*3 |
							//Then this command is write-only. Pipe_flag set to 01 (1) (write-only)
		else if (!ft_isbitset(pipe_flag, 1))
			pipe_flag = 3;	//This is a subsequent cmd which is also piped: echo 2*2 | bc | [...]
							//Pipe_flag set to 11 (3) (read-write) if it hasn't been already, otherwise no-op
	}
	else //if no pipe is found and...
	{
		if (ft_isbitset(pipe_flag, 0)) //If last command was write_only (01) or read_write (11)
			pipe_flag = 2;	//This is a final cmd which reads from pipe but is not itself piped: echo 2*2 | bc;
							//Pipe_flag set to 10 (2) (read-only)
		else if (pipe_flag == 2) //if last command was read-only and no pipe is found
			pipe_flag = 0; //reset pipe_flag to zero.
		
	}
	return (pipe_flag);
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

unsigned char	toggle_quote_flag(char quotes, unsigned char quote_flag)
{
	if (quotes == '"')
		quote_flag = ft_togglebit(quote_flag, 0);
	else if (quotes == '\'')
		quote_flag = ft_togglebit(quote_flag, 1);
	return(quote_flag);
}