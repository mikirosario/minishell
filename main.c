/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 18:17:50 by mrosario          #+#    #+#             */
/*   Updated: 2021/01/26 09:12:18 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
//#include <sys/types.h>
//#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define READLINE_BUFSIZE 1024

/*
** This function defines a buffer for reading from STDIN in steps determined by
** the READLINE_BUFSIZE constant.
**
** It will try to reserve memory for READLINE_BUFSIZE bytes. If it fails it
** will print the message pointed to by the result of strerror(errno) (should
** be no. 28 "No space left on device") and initiate program termination.
*/

char	*micli_readline(void)
{
	int		bufsize;
	int		position;
	char	*buffer;
	char	*tmp;
	int		c;

	bufsize = READLINE_BUFSIZE;
	position = 0;
	if (!(buffer = malloc(sizeof(char) * bufsize)))
	{
		printf("\n%s\n", strerror(errno));
		free(buffer);
		buffer = NULL;
		exit(EXIT_FAILURE); //gestionar con flags
	}
	while (1)
	{
		//Read a single character
		read(STDIN_FILENO, &c, 1);
		printf("%c", (char)c);
		//If we read EOF or newline was input by user, null terminate buffer
		//and return it.
		if (c == EOF || c == '\n')
		{
			buffer[position] = '\0';
			return (buffer);
		}
		//Otherwise, add character to buffer and advance one byte.
		else
			buffer[position] = c;
		position++;
		//If after advancing we exceed the allocated buffer size, expand buffer
		//by READLINE_BUFSIZE bytes and reallocate.
		//Realloc not allowed in project, so we have to do this step by step.
		//Podemos usar funciones de libft para esto, para de momento lo hago
		//un poco chapuceramente. ;p
		if (position >= bufsize)
		{
			bufsize += READLINE_BUFSIZE;
			tmp = buffer;
			if (!(buffer = malloc(sizeof(char) * bufsize)))
			{
				printf("\n%s\n", strerror(errno));
				free(tmp);
				tmp = NULL;
				exit(EXIT_FAILURE); //gestionar con flags
			}
			else
			{
				strcpy(buffer, tmp);
				buffer[position] = c;
				free(tmp);
				tmp = NULL;
			}
		}
	}
	
	return ("return");	
}

char	micli_loop(void)
{
	char shutdown;
	char *line;

	shutdown = 0;

	while (!shutdown)
	{
		write(1, ":) ", 3);
		line = micli_readline();
		if (!(strcmp(line, "exit")))
		{
				free(line);
				line = NULL;
				exit(EXIT_SUCCESS); //gestionar con flags
		}
	}

	return (0);
}

int 	main(int argc, char **argv)
{
	//config files
	(void)argc;
	(void)argv;

	//command loop
	micli_loop();

	//shutdown and cleanup
	return (0);
}