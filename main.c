/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 18:17:50 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/26 20:57:06 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/stat.h>

/*
** This function is like strlen, but for null-terminated 16 bit strings.
*/

size_t	ft_strlen16(short *str)
{
	size_t	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

/*
** PARA DEBUG
*/

void	putstr16(short *str)
{
	size_t	strlen;

	strlen = ft_strlen16(str);
	write(STDOUT_FILENO, str, strlen * sizeof(short));
	printf("\n");
}

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

short	*micli_readline(t_micli *micli, t_cmdhist *cmdhist, short **hist_stack)
{
	size_t	index;
	short	*bufsize;
	short	*char_total;
	char	move; //0 ==no move, 1== move up, -1 == move down

	//SCRATCH LOG INIT
	index = micli->cmdhist.ptrs_in_hist - 1; //scratch log
	//starting buffer size, + 3, 2 initial shorts with information on the buffer, and one null terminator
	//buf[0] == total chars in buffer, buf[1] == buffer size
	hist_stack[index] = clean_calloc(READLINE_BUFSIZE + 3, sizeof(short), micli); //scratch log reservation
	char_total = &hist_stack[index][0]; //char_total accesses index's character counter expressed in number of characters
										//present in the buffer.
	bufsize = &hist_stack[index][1]; //bufsize accesses index's buffer size expressed in number of characters that can 
									//fit in the buffer (always 3 less than actual size to make room for the two data
									//segments and the null terminator)
	*bufsize = READLINE_BUFSIZE; //record initial bufsize
								//intial char_total is 0, which will already be the default value, as we use calloc
	move = 0;
	
	while (1)
	{
		if (move)
		{
			if (move == 1 && index > 0)
				index--;
			if (move == -1 && index < cmdhist->ptrs_in_hist - 1)
				index++;
			move = 0;
			char_total = &hist_stack[index][0]; //Reset pointers
			bufsize = &hist_stack[index][1];
			write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
			write(STDOUT_FILENO, "🚀 ", 5);
			write(STDOUT_FILENO, &hist_stack[index][2], *char_total * sizeof(short));
		}
		if (!read(STDIN_FILENO, &hist_stack[index][*char_total + 2], 2)) //if read returns 0, it is EOF
		{
			write(1, "exit\n", 5);
			exit_success(micli);
		}
		*char_total += 1; //If we read a char
		//DECIDE WHETHER TO REALLOC
		//DETERMINE WHETHER CHARACTER IS PART OF AN ESCAPE SEQUENCE OR UNPRINTABLE
		//hist_stac[index][2] is the beginning of the string, after the two data segments
		if (!is_esc_seq(&hist_stack[index][2], char_total, &move))
		{
			write(STDIN_FILENO, &hist_stack[index][*char_total + 1], 2);
			if (hist_stack[index][*char_total + 1] == '\n')
			{
				//write(STDIN_FILENO, &micli->buffer[char_total - 1], 1);
				//hist_stack[index][*char_total + 1] = 0;
				*char_total -= del_from_buf(&hist_stack[index][*char_total + 1], 1); //We null the new line here, so the nulled newline makes space for another character
																					//This is why we reallocate after this section.
				//to duplicate this active line we need space for: all the chars + 2 data values + space for reading in 1 more char + 1 null terminator.
				cmdhist->active_line_size = READLINE_BUFSIZE;
				while (cmdhist->active_line_size / (*char_total + 4) == 0) //Get the minimum buffer size necessary to contain all the chars + 2 data values + space for
					cmdhist->active_line_size += READLINE_BUFSIZE;		//reading in 1 more char + 1 null terminator.
				return (hist_stack[index]);
			}
		}
		if (*char_total == *bufsize) //if we ran out of space for more characters
		{
			if (*bufsize < SHRT_MAX - 4) //O REALLOC
			{
				*bufsize += READLINE_BUFSIZE;
				hist_stack[index] = clean_realloc(hist_stack[index], (*bufsize + 3) * sizeof(short), (*char_total + 2) * sizeof(short), micli); //reallocate all characters plus two data segments to new, bigger array
				char_total = &hist_stack[index][0]; //Reset pointers
				bufsize = &hist_stack[index][1];
			}
			else //O BORRADO, SIN MEDIAS TINTAS
			{
				*char_total -= del_from_buf(&hist_stack[index][*char_total + 1], 1);
				//DEBUG CODE
				write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
				write(STDOUT_FILENO, "BAD BUNNY! NO OVERFLOW!", 23);
				sleep(1);
				write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
				write(STDOUT_FILENO, "🚀 ", 5);
				write(STDOUT_FILENO, &hist_stack[index][2], *char_total * sizeof(short));
				//DEBUG CODE	
			}
		}
	}

}

// short	*old_micli_readline(t_micli *micli)
// {
// 	short	*char_total;
// 	size_t	size;
// 	short	bufsize;
// 	size_t	index;
// 	char	move_flag; //0 ==no move, 1== move up, -1 == move down

// 	size = 0;
// 	micli->cmdhist.active_line_size = 0;
// 	bufsize = READLINE_BUFSIZE;
// 	move_flag = 0;
// 	//micli->buffer = clean_calloc(bufsize + 1, sizeof(char), micli);
// 	index = micli->cmdhist.ptrs_in_hist - 1; //char_total->position
// 	micli->cmdhist.hist_stack[index] = clean_calloc(bufsize + 3, sizeof(short), micli); //new buf [chrtotal][bufsize][~][\0]
// 	char_total = &micli->cmdhist.hist_stack[index][0]; //initial char total
// 	micli->cmdhist.hist_stack[index][1] = bufsize; //initial bufsize total
// 	//fflush(stdout);//DEBUG CODE
// 	while (1)
// 	{
// 		if (move_flag)
// 		{
// 			write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
// 			write(STDOUT_FILENO, "🚀 ", 5);
// 			if (move_flag == 1 && index > 0)
// 				index--;
// 			if (move_flag == -1 && index < micli->cmdhist.ptrs_in_hist - 1)
// 				index++;
// 			move_flag = 0;
// 			char_total = &micli->cmdhist.hist_stack[index][0];
// 			write(STDOUT_FILENO, &micli->cmdhist.hist_stack[index][2], *char_total * sizeof(short));
// 		}
// 		char_total = &micli->cmdhist.hist_stack[index][0];
// 		bufsize = micli->cmdhist.hist_stack[index][1];
// 		size += read(STDIN_FILENO, &micli->cmdhist.hist_stack[index][*char_total + 2], 2); //ESTO YA NO VALE, SE LEE CHAR POR CHAR, NO BUFSIZE POR BUFSIZE, HAY QUE MOVER EL REALLOC PARA VOLVER A PONER BUFSIZE > 1
// 		*char_total += 1;
// 		if (*char_total == bufsize)
// 		{
			
// 			micli->cmdhist.hist_stack[index] = clean_realloc(micli->cmdhist.hist_stack[index], (bufsize + READLINE_BUFSIZE + 3) * sizeof(short), (*char_total + 2) * sizeof(short), micli);
// 			char_total = &micli->cmdhist.hist_stack[index][0];
// 			micli->cmdhist.hist_stack[index][1] += READLINE_BUFSIZE;
// 		}
// 		// if (micli->buffer[char_total - 1] == '\x1b') //if escape char
// 		// {
// 		// 	escape = 1;
// 		// 	micli->buffer[--char_total] = '\0'; 
// 		// }
// 		//que pasa con write EOF se me ha olvidao????? no se escribe nada????
// 		if (!size) //si se vuelve size 0 por un ESC no cuenta como EOF :P
// 		{
// 			write(1, "exit\n", 5);
// 			exit_success(micli);
// 		}
// 		// else if (escape)//if chars are escaped, analyse in sub-buffer to determine if the sequence is an arrow key, if so, do arrow stuff and eliminate from main buffer, if not write them and continue as normal
// 		// 	escape = handle_esc_seq(micli->buffer, &char_total);
// 		else if (!is_esc_seq(micli->cmdhist.hist_stack[index], char_total, &move_flag))
// 		{
// 			write(STDIN_FILENO, &micli->cmdhist.hist_stack[index][*char_total + 1], 2);
// 			if (micli->cmdhist.hist_stack[index][*char_total + 1] == '\n')
// 			{
// 				//write(STDIN_FILENO, &micli->buffer[char_total - 1], 1);
// 				micli->cmdhist.hist_stack[index][*char_total + 1] = 0;
// 				micli->cmdhist.active_line_size = *char_total + 2;
// 				return (micli->cmdhist.hist_stack[index]);
// 			}
// 		}
		
// 		// if (micli->buffer[char_total - 1] == 'A' && micli->buffer[char_total - 2] == '[' && micli->buffer[char_total - 3] == 27)
// 		// 	printf("\nFISTRO!! PECADOR DE LA PRADERA!!!!\n");
// 		// else if (micli->buffer[char_total - 1] == 'B' && micli->buffer[char_total - 2] == '[' && micli->buffer[char_total - 3] == 27)
// 		// 	printf("\nPOR LA GLORIA DE MI MADRE!!!!\n");
// 		// else if (micli->buffer[char_total - 1] == 'C' && micli->buffer[char_total - 2] == '[' && micli->buffer[char_total - 3] == 27)
// 		// 	printf("\nHASTA LUEGO LUCAS!!!!\n");
// 		// else if (micli->buffer[char_total - 1] == 'D' && micli->buffer[char_total - 2] == '[' && micli->buffer[char_total - 3] == 27)
// 		// 	printf("\nAL ATAQUERRRL!!!!\n");
// 		//printf("BYTES READ: %zu\n", char_total);

// 	}
// }

char	micli_loop(t_micli *micli)
{
	while (1)
	{
		enable_raw_mode(&micli->raw_term, &micli->orig_term);
		signal(SIGINT, sigrun);
		//printf("🚀 ");
		write(STDOUT_FILENO, "🚀 ", 5);
		micli->active_line = micli_readline(micli, &micli->cmdhist, micli->cmdhist.hist_stack);
		//putstr16(micli->active_line);//DEBUG CODE
		micli->active_line = clean_ft_memdup(micli->active_line, micli->cmdhist.active_line_size * sizeof(short), micli);
		push_to_hist_stack(micli, micli->active_line, &micli->cmdhist);
		//cmdhist_ptr_array_alloc(micli, &micli->cmdhist);
		if (micli->active_line[2] == 0)
			micli->buffer = ft_strdup("\0");
		else
			micli->buffer = ft_short_to_strdup(&micli->active_line[2]);
		//ft_putstr(micli->buffer, ft_strlen(micli->buffer));//DEBUG CODE
		//printf("\n");
		micli->active_line = ft_del(micli->active_line);
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
	printf("\033[0;32m		               __       42Madrid\n");
	printf("\033[0;32m		  __ _  (_)___/ (_) 	Mar 2021\n");
	printf("\033[0;32m		 /  ' \\/ / __/ / /  	mrosario\n");
	printf("\033[0;32m		/_/_/_/_/\\__/_/_/   	mvillaes\033[0m\n");
	ft_bzero(&micli, sizeof(t_micli));
	micli.cmdhist.hist_stack = clean_calloc(2, sizeof(short *), &micli);
	micli.cmdhist.ptrs_in_hist = 1;
	micli.cmdhist.cmdhist_buf = 1;
	micli.envp = ft_envdup(envp, &micli);
	tcgetattr(STDIN_FILENO, &micli.orig_term);
	norminette_made_me_do_it(&micli);
	delete_oldpwd(&micli);
	micli_loop(&micli);
	return (0);
}
