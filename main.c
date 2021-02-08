/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 18:17:50 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/08 15:54:48 by mvillaes         ###   ########.fr       */
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

char	*find(char *line, char *line_end, char c)
{
	if (line)
	{
		if (line_end && line_end > line)
			while (line < line_end && *line != c)
				line++;
		else
			while (*line && *line != c)
				line++;
	}
	return (line);
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
** Note that read will NOT clear the high bytes of the memory location passed
** as the second argument if the chunk to be copied is smaller than specified
** in the third argument, so the value copied will not be as expected unless
** the high bytes are zeroed.
*/

char	*micli_readline(t_micli *micli)
{
	size_t	size;

	size = 0;
	micli->bufsize = READLINE_BUFSIZE;
	micli->position = 0;
	micli->buffer = clean_calloc(micli->bufsize, sizeof(char), micli);
	while (1)
	{
		size += read(STDIN_FILENO, &micli->c, 1); //Si no ha leído nada se comporta como salto de línea?? O_O Lee último carácter, salto de línea? Se queda colgado en read???
		//If we read EOF or newline was input by user, null terminate buffer, print and return it.
		if (micli->c == EOF || micli->c == '\n')
		{
			micli->buffer[micli->position] = '\0';
			//ft_printf("%s\n", micli->buffer); //esto imprime último comando a stdout
			return (micli->buffer);
		}
		//Otherwise, add the character to our buffer and advance one byte.
		else
			micli->buffer[micli->position] = micli->c;
		micli->position++;
		//If after advancing we exceed the allocated buffer size, expand buffer by READLINE_BUFSIZE bytes and reallocate.
		//Podemos usar funciones de libft para esto, para de momento lo hago un poco chapuceramente. ;p
		if (micli->position >= micli->bufsize)
		{
			micli->bufsize += READLINE_BUFSIZE;
			if (!(micli->buffer = ft_realloc(micli->buffer, micli->bufsize, micli)))
				exit_failure(micli);
		}
	}
}

// typedef void (*sighandler_t)(int);

// sighandler_t signal(int signum, sighandler_t handler);
//
//first arg is the signal number, each signal has a predefined integer value assigned.
//ex: 2 = SIGINT and 20 SIGTSTP
//second arg is a pointer to a singal handler. The signal handler is a function that
//must accept an int as the only parameter and return void as descibed by the typedef

// void sigint_handler()
// {
//     printf("Caught crtl-c\n");
// 	signal(SIGINT, SIG_IGN);
// }

// //ctrl+C interrupt the program and terminates the application
void	signal_c()
{
	(void) signal(SIGINT, SIG_DFL);
}
// //ctrl+\
// tells the application to exit as soon as possible without saving anything;

void	singal_barra()
{
	(void) signal(SIGQUIT, SIG_DFL);
}

// ctrl+D generates a EOF/end of imput (normally EOF=-1)
void	signal_d()
{
	//send EOF (?)
}

char	micli_loop(t_micli *micli)
{
	char shutdown;
	
	shutdown = 0;

	while (!shutdown)//no parece que esté usando shutdown...
	{
		write(1, "🚀 ", 6);
		micli->buffer = micli_readline(micli);//this is redundant, as the function returns micli->buffer, leaving it here for clarity
		tokenize(micli->buffer, micli);
		micli->buffer = ft_del(micli->buffer);
	}
	return (0);
}

int 	main(int argc, char **argv, char **envp)
{
	t_micli micli;

	ft_bzero(&micli, sizeof(t_micli));
	micli.envp = envp;
	micli.builtin_strlen = ft_strlen(BUILTINS);
	//config files
	(void)argc;
	(void)argv;


	//command loop
	micli_loop(&micli);

	//shutdown and cleanup
	return (0);
}
