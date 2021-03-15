/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 18:17:50 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/15 22:16:12 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
		if (!size)
		{
			write(1, "exit\n", 5);
			exit_success(micli);
		}
		else if (micli->buffer[size - 1] == '\n')
		{
			micli->buffer[size - 1] = '\0';
			return (micli->buffer);
		}
		bufsize += READLINE_BUFSIZE;
		if (!(micli->buffer = ft_realloc(micli->buffer, bufsize, micli)))
			exit_failure(micli);
	}
}

char	micli_loop(t_micli *micli)
{
	while (1)
	{
		signal(SIGINT, sigrun);
		write(STDOUT_FILENO, "🚀 ", 5);
		micli->buffer = micli_readline(micli);
		process_raw_line(micli->buffer, micli);
		micli->buffer = ft_del(micli->buffer);
		signal(SIGQUIT, sigrun);
	}
	return (0);
}

int		main(int argc, char **argv, char **envp)
{
	t_micli micli;

	(void)argc;
	(void)argv;
	ft_bzero(&micli, sizeof(t_micli));
	micli.envp = ft_envdup(envp, &micli);
	delete_oldpwd(&micli);
	micli_loop(&micli);
	return (0);
}
