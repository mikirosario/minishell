/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 18:17:50 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/08 23:02:14 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Finds first 'c' in line and return its address.
**
** If passed a null pointer, returns null.
**
** If line_end is passed and is greater than line, find will search only until
** the specified line_end. Otherwise, it will search until it finds a NULL char.
** 
** NOT BEING USED...
*/

// char	*find(char *line, char *line_end, char c)
// {
// 	if (line)
// 	{
// 		if (line_end && line_end > line)
// 			while (line < line_end && *line != c)
// 				line++;
// 		else
// 			while (*line && *line != c)
// 				line++;
// 	}
// 	return (line);
// }

/*
** This function defines a buffer for reading from STDIN in steps determined by
** the READLINE_BUFSIZE constant.
**
** It will try to reserve memory for READLINE_BUFSIZE bytes. If it fails it
** will print the message pointed to by the result of strerror(errno) (should
** be no. 28 "No space left on device") and initiate program termination.
**
** When reading from STDIN the read() function hangs until '\n' newline or
** terminal EOF is input, after which it will read the first thing in its
** buffer in a chunk defined by the third argument and copy it to the address
** passed as the second argument. For every subsequent call it will read the
** subsequent chunk in the buffer until the newline or EOF. If the third
** argument defines a chunk size that is larger than what remains
** in the buffer to be read, the smaller size will be read. After reaching
** newline or EOF it will hang again pending another newline or EOF. Newline
** counts as a character.
**
** This function has been rewritten from the initial loop that more closely
** mirrored an online tutorial to better use the read buffer. The read function
** will now read up to READLINE_BUFSIZE bytes from stdout at a time and copy
** them to the buffer. The number of bytes read are returned and stored in the
** size variable.
**
** If READLINE_BUFSIZE bytes have been copied to the buffer and EOF or new line
** is not found at buffer[size - 1] (transforming size to position to point to
** the last copied byte in the buffer), there is more left to be copied, so we
** reallocate the buffer, increasing its size by a further READLINE_BUFSIZE
** bytes, and then read again, adding the number of bytes read to the prior
** number of bytes read. Once EOF or new line is found, it is replaced with a
** null terminator and the function returns the address of the buffer for
** parsing.
**
*/

char	*micli_readline(t_micli *micli)
{
	size_t	size;
	size_t	bufsize;

	size = 0;
	bufsize = READLINE_BUFSIZE;
	micli->buffer = clean_calloc(bufsize, sizeof(char), micli);
	while (1)
	{
		size += read(STDIN_FILENO, &micli->buffer[size], READLINE_BUFSIZE);
		//If we read EOF or newline was input by user, null terminate buffer.
		if (micli->buffer[size - 1] == EOF || micli->buffer[size - 1] == '\n')
		{
			micli->buffer[size - 1] = '\0'; //Convert EOF or '\n' to null termination. We'll need to handle EOF differently as it means ctrl'/'
			//ft_printf("%u, %s\n", size, micli->buffer); //esto imprime última línea a stdout
			return (micli->buffer);
		}
		bufsize += READLINE_BUFSIZE;
		if (!(micli->buffer = ft_realloc(micli->buffer, bufsize, micli)))
			exit_failure(micli);		
	}
}

char	micli_loop(t_micli *micli)
{
	char shutdown;
	
	shutdown = 0;

	while (!shutdown)//no parece que esté usando shutdown...
	{
		write(STDOUT_FILENO, "🚀 ", 5);
		micli->buffer = micli_readline(micli);//this is redundant, as the function returns micli->buffer, leaving it here for clarity
		process_raw_line(micli->buffer, micli);
		micli->buffer = ft_del(micli->buffer);
	}
	return (0);
}

int 	main(int argc, char **argv, char **envp)
{
	t_micli micli;

	ft_bzero(&micli, sizeof(t_micli));
	// pipe(&micli.pipe[0]);
	// pipe(&micli.pipe[2]);
	// pipe(&micli.pipe[4]);
	// close(micli.pipe[5]);
	micli.envp = ft_envdup(envp, &micli);
	//micli.builtin_strlen = ft_strlen(BUILTINS);
	//config files
	(void)argc;
	(void)argv;

	//signal
	catch_signal();
	//command loop
	micli_loop(&micli);

	//shutdown and cleanup
	return (0);
}
