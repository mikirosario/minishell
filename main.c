/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 18:17:50 by mrosario          #+#    #+#             */
/*   Updated: 2021/01/26 20:37:55 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function reallocates the memory of the string pointed to by ptr to a
** new memory block of the size defined by size, freeing the old memory block.
**
** If the reallocation fails, the old memory block is destroyed and a null
** pointer is returned.
**
** This function uses memcpy, which is not safe if dst and src overlap.
**
** Behaviour is undefined if size is less than the memory to be reallocated.
** Probably means segfault. ;)
*/

char	*ft_realloc(char *ptr, size_t size)
{
	char *tmp;

	tmp = ptr;
	if (!(ptr = malloc(sizeof(char) * size)))
	{
		printf("\n%s\n", strerror(errno));
		free(tmp);
		tmp = NULL;
	}
	else
	{
		memcpy(ptr, tmp, size);
		free(tmp);
		tmp = NULL;
		//printf("Bufsize: %zu\n", size);
	}
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
		printf("\n%s\n", strerror(errno));
		free(micli->buffer);
		micli->buffer = NULL;
		exit(EXIT_FAILURE); //gestionar con flags
	}
	while (1)
	{
		size += read(STDIN_FILENO, &micli->c, 1); //Si no ha leído nada se comporta como salto de línea?? O_O Lee último carácter, salto de línea? Se queda colgado en read???
		//If we read EOF or newline was input by user, null terminate buffer, print and return it.
		if (micli->c == EOF || micli->c == '\n')
		{
			micli->buffer[micli->position] = '\0';
			printf("%s\n", micli->buffer);
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
			if (!(micli->buffer = ft_realloc(micli->buffer, micli->bufsize)))
				exit(EXIT_FAILURE); //realloc failure gestionar con flags
		}
	}
	
	return ("return");	
}

char	micli_loop(t_micli *micli)
{
	char shutdown;
	char *line;

	shutdown = 0;

	while (!shutdown)//no parece que esté usando shutdown...
	{
		//write(1, "TEST", 4);
		write(1, "🚀 ", 6);
		line = micli_readline(micli);//stays here...
		//write(1, "TEST", 4);
		if (!(strcmp(line, "exit")) || !(strcmp(line, "quit")))
		{
				free(line);
				line = NULL;
				exit(EXIT_SUCCESS); //gestionar con flags
		}
		free(line);
		line = NULL;
	}

	return (0);
}

int 	main(int argc, char **argv)
{
	t_micli micli;

	bzero(&micli, sizeof(t_micli));
	//config files
	(void)argc;
	(void)argv;

	//command loop
	micli_loop(&micli);

	//shutdown and cleanup
	return (0);
}