/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/18 12:14:45 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/17 01:56:06 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function will attempt to close all open pipes in the pipe array. If any
** close fails, errno will be recorded and the function will return a sys error.
** Otherwise the function will exit with 1.
*/

int		close_pipes(size_t fd_num, int *pipes_array, t_micli *micli)
{
	size_t	i;

	i = 0;
	while (i < fd_num) // Try to close any open fds in pipeline.
	{
		if (close(pipes_array[i]) == -1)
		{
			micli->syserror = errno;
			sys_error(micli);
		}
		i++;
	}
	return (1);
}

/*
** This function will close any pipes in the pipe struct and free the memory
** occupied by their file descriptors, and then zero all the data inside.
** Nothing better than clearing your pipes.
*/

void	clear_pipes(t_pipes *pipes, t_micli *micli)
{
	if (pipes->array) //Free pipe array if already reserved
	{
		close_pipes(pipes->array_size, pipes->array, micli);
		pipes->array = ft_del(pipes->array); //Free pipe array...
		ft_bzero(pipes, sizeof(t_pipes));
	}
}

/*
** This function resets the pipeline used by piped commands in the command line.
**
** The pipe index is reset to 0.
**
** If an array of pipes has already been reserved, it is freed and the pointer
** set to NULL.
**
** The array size is calculated as (pipes->count) * 2. The number of pipes is
** multiplied by two to derive the number of file descriptors (2 per pipe). The
** array size == the total fd count.
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
** The pipe count is 2, so the number of pipes in the pipeline will be 2, and
** the number of file descriptors will be 4. Thus, the array looks like this:
**
**				pipe0			pipe1
**			   0	 1		   2	 3
** pipes	[read][write]	[read][write]
**
** The pipeline, when in use, will look like this:
**
**				pipe0	 		pipe1
**			[write][read]	[write][read]
** stdin	1			0	3			2	stdout
**		↓	↑	   		↓	↑	  		↓	↑
**	  	cmd0			cmd1			cmd2
**
** For a given command, the proper write_fd position in the pipe array is given
** by the equation write_fd = cmd_index * 2 + 1. For example, for cmd0, the
** proper write_fd position is 0 * 2 + 1 = 1. For cmd1 the proper write_fd
** position is 1 * 2 + 1 = 3. For cmd2 the proper write_fd is 2 * 2 + 1 = 5,
** which would cause a buffer overflow if used. :)
**
** The proper read_fd position in the pipe array is given by write_fd - 3 (that
** is, the read fd of the preceding pipe). For cmd2 that is 5 - 3 = 2. For cmd1
** that is 3 - 3 = 0. For cmd0 that is 1 - 3 == -2, which causes a sign
** overflow.
**
** Note that for cmd0 there will be a sign overflow, but this is okay, because
** the read_fd for the first command in a pipeline will never actually be used
** Similarly, the calculated write_fd position for the last command in a
** pipeline will be outside the reserved array, but will likewise never be used.
** This is because of the pipe flag. The precise mechanism for preventing this
** with the pipe flag is explained in cmd_execution.c.
**
** Also note that this method will cause an overflow in a pipe array of size
** SIZE_MAX (that is, with SIZE_MAX / 2 pipes). Therefore, if the pipe count is
** greater than this limit, we prohibit this pipeline to avoid a crash.
** Aren't we so nice? :)
*/

int		pipe_reset(t_pipes *pipes, t_micli *micli)
{
	size_t	i;

	i = 0;
	if (pipes->count > PIPE_MAX)
		exit_failure(micli); // MICLI FAILS!!!!! xD Maybe a little too drastic... should return to command line with error message, but that's not implemented yet.. xD	
	pipes->array_size = (pipes->count) * 2;
	pipes->array = clean_calloc(pipes->array_size, sizeof(int), micli); //Reserve pipe_count + 2 pipes, maybe stick this value in 'fdcount'.
	while (i < pipes->array_size) //create all pipes
	{
		if (pipe(&pipes->array[i]) == -1) //in case of pipe creation failure
		{
			micli->syserror = errno;
			close_pipes(i, pipes->array, micli);
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