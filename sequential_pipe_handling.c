/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sequential_pipe_handling.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/18 12:14:45 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/20 17:02:31 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function selects which pipe's write file descriptor to close. It will
** close the write file descriptor of the pipe used by the preceding command.
** This appears to 'de-pipe' the pipe, maybe by turning it into a normal file
** with EOF. This will stop the current command from waiting for further input
** from the pipe inode, causing it to close after reading.
**
** For example, if current command is cmd2:
**
**			pipe1	    	pipe2	 		pipe3
**	 ...[write][read]	[write][read]	[write][read]...
**		5			0	1			2	3			4		
**		x			↓	↑	   		↓	↑	  		↓
**	...cmd1		  	cmd2			cmd3			cmd1...
*/

void	close_write_end_preceding_pipe(unsigned char pipe_reset_flag, int *pipes)
{
	if (pipe_reset_flag == 0) // If this flag is 0, we read from 4, write to 1, so close 5
		close(pipes[5]); // Close writepd 5
	else if (pipe_reset_flag == 1) //If this flag is 1, we read from 0, write to 3, so close 1
		close(pipes[1]); //Close writepd 1
	else								//If this flag is 2, we read from 2, write to 5, so close 3
		close(pipes[3]); //Close writepd 3s
}

/*
** This function selects which two of the three available pipes will be used by
** the piped command for writing and reading. Data will be read from the pipe
** previously used for writing, and written to the next pipe after it.
**
**			pipe1  		pipe2		 pipe3	
**		  0		1 	   2	 3	 	4	  6	 
** pipe[read][write][read][write][read][write]
**
**
**
**			pipe1	    	pipe2	 		pipe3
**	 ...[write][read]	[write][read]	[write][read]...
**		  ↑			↓	↑	   		↓	↑	  		↓
**	...cmd1		  	cmd2			cmd3			cmd1...
*/

void	pipe_selector(unsigned char pipe_reset_flag, int *writepos, int *readpos)
{
	if (pipe_reset_flag == 0)
	{
		*writepos = 1;
		*readpos = 4;
	}
	else if (pipe_reset_flag == 1)
	{
		*writepos = 3;
		*readpos = 0;
	}
	else
	{
		*writepos = 5;
		*readpos = 2;
	}
}

/*
** This function closes the read end of the pipe that will not be used in the
** next piped command, which will be the last reference to that pipe descriptor,
** given that the write end of the pipe should already have been closed before,
** causing the associated file descriptor to be eliminated. It then creates a
** new pipe to replace the closed one, saving the new pipe's read and write fd
** numbers in the same place in the array as the old one.
**
** This 'reset' pipe will then be used in the command after the next one. Below
** is a simple schematic of what the function is supposed to accomplish:
**
**								pipe inode
**									|->inode refs 1
**							  file descriptor
**	read refs: 1 (pipe[i])	<-|total refs 1 |-> write refs 0
**
** 	close(pipe[i])			----> read refs 0.
**	read refs 0				----> total refs 0.
**	total refs 0 			----> remove file descriptor.
**	remove file descriptor	----> inode refs 0.
**	inode refs 0			----> remove inode.
**	pipe(pipe[i])			----> create new pipe ------|
**														|
**								pipe inode<-------------|
**									|->inode refs 1
**							   file descriptor
**	read refs: 1 (pipe[i])	<-|total refs 1 |-> write refs 1 (pipe[i+1])
**
** This function takes a pointer to a three-pipe array as its second argument.
** Each pipe is divided into read and write elements. Thus:
**
**		   pipe1		pipe2		 pipe3
**		  0		1	   2	 3		4	  6	
** pipe[read][write][read][write][read][write]
**
** The pipe_reset_flag cycles between 0, 1 and 2 every time a piped command is
** executed, and thus indicates which pipe is currently unused and needs to be
** reset.
**
** In state 0, the second pipe (pipe[2][3]) is unused, in state 1 the third pipe
** (pipe[4][5]) is unused, in state 2 the first pipe (pipe[0][1]) is unused.
**
** If one of the three success conditions is met, 1 is returned. Otherwise 0 is
** returned.
*/

int		pipe_reset(unsigned char pipe_reset_flag, int *pipes)
{
	int	ret;

	if (pipe_reset_flag == 0 && !close(pipes[2]) && !pipe(&pipes[2])) // If pipe_reset_flag is 0 and close(pipe[2]) is successful and new pipe creation is successful
		ret = 1;
	else if (pipe_reset_flag == 1 && !close(pipes[4]) && !pipe(&pipes[4])) //If pipe_reset_flag is 1 and close(pipe[4]) is successful and new pipe creation is successful
		ret = 1;
	else if (pipe_reset_flag == 2 && !close(pipes[0]) && !pipe(&pipes[0]))		//If pipe_reset_flag is 2 and close(pipe[0]) is successful and new pipe creation is successful
		ret = 1;
	else
		ret = 0;
	return(ret);
}

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