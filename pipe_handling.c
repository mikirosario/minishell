/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/18 12:14:45 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/24 21:56:41 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function will attempt to close all open pipes in the pipe array. Any
** close fails, errno will be recorded and the function will exit with 0.
** Otherwise the function will exit with 1.
*/

int		close_pipes(size_t pipe_num, t_pipes *pipes, t_micli *micli)
{
	size_t	i;

	i = 0;
	while (i < pipe_num) // Try to close any open pipes.
	{
		if (close(pipes->array[i]) == -1)
		{
			micli->syserror = errno;
			return (0);
		}
		i += 2;
	}
	return (1);
}

/*
** This function resets the pipeline used by piped commands in the command line.
**
** The pipe index is reset to 0.
**
** If an array of pipes has already been reserved, it is freed and the pointer
** set to NULL.
**
** The array size is calculated as (pipes->count + 1) * 2. One extra pipe is
** added to the array to cap the pipeline at the end (see below), and the number
** is multiplied by two to derive the number of file descriptors (2 per pipe).
** The array size == the total fd count.
**
** An integer array of the calculated size is reserved to hold the file
** descriptor numbers of each pipe.
**
** Pipes are created for every file descriptor pair in the array.
**
** For example, for the command lines:
**
** echo 2*2 | bc | cat
**
** The pipe count is 2, and one additional pipe is added at the end, so the
** number of pipes in the pipeline will be 3, and the number of file descriptors
** will be 6. Thus, the array looks like this:
**
**				pipe0			pipe1			pipe2
**			   0	 1		   2	 3		   4	 5
** pipes	[read][write]	[read][write]	[read][write]
**
** The pipeline, when in use, will look like this:
**
**		    	pipe0	 		pipe1			pipe1
**			[write][read]	[write][read]	[write][read]
** stdout	1			0	3			2	5			4	stdin
**		↓	↑	   		↓	↑	  		↓	↑			↓	↑
**	  	cmd1			cmd2			cmd3			cmd4
**
** For a given command, the proper write_fd is currently given by the equation
** write_fd = pipe_num * 2 + 1.
**
** The proper fd for reading is currently given by write_fd - 3 (that is, the
** read fd of the preceding pipe).
** 
** Note that for pipe 0 this will cause an overflow, but this is okay, because
** the read_fd for the first command in a pipeline will never actually be used
** (the mechanism for preventing this is explained in cmd_execution.c).
** Similarly, the calculated write_fd for the last command in a pipeline will
** be outside the array, but will likewise never be used.
**
** Also note that this method will cause an overflow in a pipeline of size
** SIZE_MAX + 2 (that is, with SIZE_MAX / 2 - 1 pipes). Therefore, if the pipe
** count is greater than this limit, we prohibit this pipeline to avoid a crash.
** Aren't we so nice? :) MANUUUUU, COMPRUEBA MIS MATES... O_O
*/

int		pipe_reset(t_pipes *pipes, t_micli *micli)
{
	size_t	i;

	i = 0;
	pipes->index = 0;
	if (pipes->count == PIPE_MAX)
		exit_failure(micli); // MICLI FAILS!!!!! xD Maybe a little too drastic... should return to command line with error message, but that's not implemented yet.. xD
	if (pipes->array) //Free pipe array if already reserved
		pipes->array = ft_del(pipes->array); //Free pipe array...
	pipes->array_size = (pipes->count + 1) * 2;
	pipes->array = clean_calloc(pipes->array_size, sizeof(int), micli); //Reserve pipe_count + 2 pipes, maybe stick this value in 'fdcount'.
	while (i < pipes->array_size) //create all pipes
	{
		if (pipe(&pipes->array[i]) == -1) //in case of pipe creation failure
		{
			micli->syserror = errno;
			close_pipes(i, pipes, micli);
			exit_failure(micli); // MICLI FAILS!!!!! xD Maybe a little too drastic... should return to command line with error message, but that's not implemented yet.. xD
			//NOTE: If pipe creation failed, errno will reflect this. If pipe creation failed and then close pipes failed, errno will report close pipe failure.
		}
		i += 2;
	}
	return (1);
}

/*
** Count number of pipes in a line until the next ';' or EOL.
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
		escape_flag = 0;														//Status Condition
		//Note that single quotes prevent escaping.
		if (*line == '\\' && quote_flag != 2 &&	!(quote_flag == 1				//If escape is found and the following two expressions are NOT true:
		&& ft_memchr(DQUOTE_ESC_CHARS, *(line + 1), 3) == NULL))					//1. We are between single quotes.
		{																			//2. We are between double quotes, but an escapable character has not been found after the escape.
			escape_flag = 1;													//Escape flag is set.
			line++;																//Line is incremented by one to examine escaped character.
		}
		if ((*line == '"' && !escape_flag && (!quote_flag || quote_flag == 1)) 	//If double quotes are found and the double quotes have not been escaped and we are either not between quotes or we are between double quotes...
		|| (*line == '\'' && (quote_flag == 2 || (!escape_flag && !quote_flag))))	//OR if single quote is found and we are between single quotes or the single quote has not been escaped and we are not between quotes.
			quote_flag = toggle_quote_flag(*line, quote_flag);					//Toggle the quote flag appropriately. Quote flag will toggle when: no quotes and quotes found or quotes and unescaped quotes of same type found.
		if (*line == ';' && !quote_flag && !escape_flag)						//If semicolon is found and we aren't between any quotes and the semicolon hasn't been escaped
			break ;																//Leave the while and return pipe count (we only count pipes until the next semicolon or EOL)
		if (*line == '|' && !quote_flag && !escape_flag)						//If pipe is found and we aren't between any quotes and the pipe hasn't been escaped...
			pipe_count++;														//Count the pipe.
		line++;																	//Go to next char.
	}
	return (pipe_count);
}