/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 18:17:50 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/20 21:17:02 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function is the poster child of things that only exist because of the
** school norm. In this case, the school norm prohibits using operations to
** define constants in the header like normal human beings. Actually, it doesn't
** even recognize constants as constants if you do.
**
** So I have no choice but to define all my constants that are a result of maths
** or bitwise operations here, at run time. All variable, structure and function
** names are in honour of Norminette, French feline and royal pain in the arse.
**
** *le eyeroll*
*/

void	norminette_made_me_do_it(t_micli *micli)
{
	ft_bzero(&micli->tonti, sizeof(t_normis_fault));
	micli->tonti.pipe_max = __SIZE_MAX__ / 2;
	micli->tonti.perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	micli->tonti.f_tr = O_WRONLY | O_CREAT | O_TRUNC;
	micli->tonti.f_ap = O_WRONLY | O_CREAT | O_APPEND;
	micli->tonti.f_re = O_RDONLY | O_CREAT;
}

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

int	main(int argc, char **argv, char **envp)
{
	t_micli	micli;

	(void)argc;
	(void)argv;
	printf("\033[0;32m		               __       42Madrid\n\033[0m");
	printf("\033[0;32m		  __ _  (_)___/ (_) 	Mar 2021\n\033[0m");
	printf("\033[0;32m		 /  ' \\/ / __/ / /  	mrosario\n\033[0m");
	printf("\033[0;32m		/_/_/_/_/\\__/_/_/   	mvillaes\n\033[0m");
	bzero(&micli, sizeof(t_micli));
	micli.micli_loop = micli_loop;
	norminette_made_me_do_it(&micli);
	micli.envp = ft_envdup(envp, &micli);
	delete_oldpwd(&micli);
	micli_loop(&micli);
	return (0);
}
