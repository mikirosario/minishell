/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_buffer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/19 05:36:10 by miki              #+#    #+#             */
/*   Updated: 2021/03/19 06:45:49 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string.h>
#include <stdio.h>


int	main(void)
{
	int buf_pipe[2];
	int	desvio[2];
	int	my_stdout;
	char	buf[10];

	//read:write
	if (isatty(STDOUT_FILENO))
		printf("%s\n", ttyname(STDOUT_FILENO));
	desvio[0] = dup(STDIN_FILENO);
	desvio[1] = dup(STDOUT_FILENO);
	pipe(buf_pipe);
	dup2(buf_pipe[0], STDIN_FILENO);
	dup2(buf_pipe[1], STDOUT_FILENO);
	close(buf_pipe[0]);
	close(buf_pipe[1]);
	// dup2(STDIN_FILENO, buf_pipe[0]);
	// close(STDIN_FILENO);
		//write(STDOUT_FILENO, "test", 4);
		while (1)
		{
			read(STDIN_FILENO, buf, 10);
			if (buf[0] == 't')
				write(desvio[1], buf, strlen(buf));
			else if (buf[0] == 'e')
				break ;
		}
		//read(buf_pipe[0], buf, 10);
		//write(buf_pipe[0], buf, 5);
		return (1);
}