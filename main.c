/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 18:17:50 by mrosario          #+#    #+#             */
/*   Updated: 2021/01/28 19:35:09 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Any program failure leading to program termination should immediately save
** errno to the micli->syserror variable, clean up as needed, and then call this
** function.
**
** This function will print the appropriate error message and kill the program.
**
** If the program fails due to an internal error, rather than a system error,
** the message "Unknown fatal error" will be displayed.
*/

void	exit_failure(t_micli *micli)
{
	if (micli->syserror)
		ft_printf("\n%s\n", strerror(micli->syserror)); //make ft_realloc set errno, or use internal error handling :p
	else
		ft_printf("\nUnknown fatal error\n");
	exit(EXIT_FAILURE);
}

/*
** This function reallocates the memory of the string pointed to by ptr to a
** new memory block of the size defined by size, freeing the old memory block.
** 
** If a null pointer is passed, a null pointer will be returned and nothing
** will be freed. Freeing a null pointer results in no operation being
** performed, so it's fine.
**
** If the reallocation fails, the old memory block is destroyed and a null
** pointer is returned. Errno at failure is saved to a variable in the micli
** struct for use in subsequent error handling.
**
** This function uses memcpy, which is not safe if dst and src overlap.
**
** Behaviour is undefined if size is less than the memory to be reallocated.
** Probably means segfault. So just don't do that. ;)
*/

char	*ft_realloc(char *ptr, size_t size, t_micli *micli)
{
	char *tmp;

	tmp = ptr;
	if (!ptr || !(ptr = malloc(sizeof(char) * size)))
		micli->syserror = errno;
	else
		ft_memcpy(ptr, tmp, size);
	//printf("Bufsize: %zu\n", size); (Debug code)
	tmp = ft_del(tmp);
	return (ptr);
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
	if (!(micli->buffer = malloc(sizeof(char) * micli->bufsize)))
	{
		micli->syserror = errno;
		micli->buffer = ft_del(micli->buffer);
		exit_failure(micli); //gestionar con flags
	}
	while (1)
	{
		size += read(STDIN_FILENO, &micli->c, 1); //Si no ha leído nada se comporta como salto de línea?? O_O Lee último carácter, salto de línea? Se queda colgado en read???
		//If we read EOF or newline was input by user, null terminate buffer, print and return it.
		if (micli->c == EOF || micli->c == '\n')
		{
			micli->buffer[micli->position] = '\0';
			ft_printf("%s\n", micli->buffer); //esto imprime último comando a stdout
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
	
	return ("return");	
}

char	micli_loop(t_micli *micli)
{
	char shutdown;
	char *line;
	DIR *d;
	struct dirent *dir;
	
	d = opendir(".");
	shutdown = 0;

	while (!shutdown)//no parece que esté usando shutdown...
	{
		//write(1, "TEST", 4);
		write(1, "🚀 ", 6);
		line = micli_readline(micli);//stays here...
		//write(1, "TEST", 4);
		if (!(ft_strcmp(line, "exit")))
		{
				line = ft_del(line);
				exit(EXIT_SUCCESS); //gestionar con flags
		}
		//ls "implementation" ;)
		if (!(ft_strcmp(line, "ls")))
		{
			if (d)
			{
				while ((dir = readdir(d)) != NULL)
					ft_printf("%s\n", dir->d_name);
				closedir(d);
			}
		}
		line = ft_del(line);
	}
	return (0);
}

int 	main(int argc, char **argv)
{
	t_micli micli;

	ft_bzero(&micli, sizeof(t_micli));
	//config files
	(void)argc;
	(void)argv;

	//command loop
	micli_loop(&micli);

	//shutdown and cleanup
	return (0);
}
