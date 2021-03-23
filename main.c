/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 18:17:50 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/23 22:32:39 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


/*
** Convert short to char buffer. Discard insigificant bytes. Duplicate
** significant bytes. Return duplicate. If there were no significant bytes,
** return NULL.
*/

char	*short_to_chars(short short_char)
{
	unsigned char	charbuf[3];
	char			*char_index;
	int				i;

	ft_bzero(charbuf, 3);
	i = 0;
	charbuf[0] = short_char & 0xff;
	charbuf[1] = (short_char >> 8) & 0xff;
	char_index = (char *)charbuf;
	while (i++ < 2)
		if (!(*char_index))
			char_index++;
	if (*char_index)
		return (ft_strdup(char_index));
	return (NULL);
}

/*
** This frankenfunction accepts a null-terminated string of unsigned ints and
** spits out a null_terminated string of chars, eliminating all the padding
** bytes, no questions asked. If the operation fails, NULL is returned.
**
** WARNING: For little-endian systems only!
**
** EL PLAN BORRÓN!!!! Mwajajajaja. xD
*/

char	*ft_short_to_strdup(short *short_str)
{
	size_t	strlen16;
	size_t 	i;
	char	*delete;
	char	*char_str;
	char	*short_char;

	char_str = NULL;
	if (!short_str)
		return (NULL);
	strlen16 = ft_strlen16(short_str);
	i = 0;
	while (i < strlen16)
	{
		delete = char_str;
		short_char = short_to_chars((unsigned short)short_str[i]);
		if (short_char)
		{
			if (char_str)
				char_str = ft_strjoin(char_str, short_char);
			else
				char_str = ft_strdup(short_char);
			if (delete)
				free(delete);
			free(short_char);
		}
		i++;
	}
	return (char_str);
}


/*
** This function is like strlen, but for null-terminated 32 bit strings.
*/

size_t ft_strlen16(short *str)
{
	size_t i;

	i = 0;
	while (str[i])
		i++;
	return (i);
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

short	*micli_readline(t_micli *micli)
{
	size_t	char_total;
	size_t	size;
	size_t	bufsize;
	size_t	index;
	char	move_flag; //0 ==no move, 1== move up, -1 == move down

	char_total = 0;
	size = 0;
	micli->cmdhist.active_line_size = 0;
	bufsize = READLINE_BUFSIZE;
	move_flag = 0;
	//micli->buffer = clean_calloc(bufsize + 1, sizeof(char), micli);
	index = micli->cmdhist.ptrs_in_hist - 1; //char_total->position
	micli->cmdhist.hist_stack[index] = clean_calloc(bufsize + 1, sizeof(short), micli);
	while (1)
	{
		if (move_flag)
		{
			write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
			write(STDOUT_FILENO, "🚀 ", 5);
			if (move_flag == 1 && index > 0)
				index--;
			if (move_flag == -1 && index < micli->cmdhist.ptrs_in_hist - 1)
				index++;
			move_flag = 0;
			if (*micli->cmdhist.hist_stack[index] == '\0')
				char_total = 0;
			else
				char_total = ft_strlen16(micli->cmdhist.hist_stack[index]);
			write(STDOUT_FILENO, micli->cmdhist.hist_stack[index], char_total * sizeof(short));
		}
		size += read(STDIN_FILENO, &micli->cmdhist.hist_stack[index][char_total], 2); //ESTO YA NO VALE, SE LEE CHAR POR CHAR, NO BUFSIZE POR BUFSIZE, HAY QUE MOVER EL REALLOC PARA VOLVER A PONER BUFSIZE > 1
		char_total++;
		// if (micli->buffer[char_total - 1] == '\x1b') //if escape char
		// {
		// 	escape = 1;
		// 	micli->buffer[--char_total] = '\0'; 
		// }
		//que pasa con write EOF se me ha olvidao????? no se escribe nada????
		if (!size) //si se vuelve size 0 por un ESC no cuenta como EOF :P
		{
			write(1, "exit\n", 5);
			exit_success(micli);
		}
		// else if (escape)//if chars are escaped, analyse in sub-buffer to determine if the sequence is an arrow key, if so, do arrow stuff and eliminate from main buffer, if not write them and continue as normal
		// 	escape = handle_esc_seq(micli->buffer, &char_total);
		else if (!is_esc_seq(micli->cmdhist.hist_stack[index], &char_total, &move_flag))
		{
			write(STDIN_FILENO, &micli->cmdhist.hist_stack[index][char_total - 1], 2);
			if (micli->cmdhist.hist_stack[index][char_total - 1] == '\n')
			{
				//write(STDIN_FILENO, &micli->buffer[char_total - 1], 1);
				micli->cmdhist.hist_stack[index][char_total - 1] = 0;
				micli->cmdhist.active_line_size = char_total;
				return (micli->cmdhist.hist_stack[index]);
			}
		}
		if (char_total == bufsize)
		{
			bufsize += READLINE_BUFSIZE;
			micli->cmdhist.hist_stack[index] = ft_realloc(micli->cmdhist.hist_stack[index], (bufsize + 1) * sizeof(short), (bufsize - READLINE_BUFSIZE) * sizeof(short), micli);
		}
		if (!micli->cmdhist.hist_stack[index])
			exit_failure(micli);
		
		// if (micli->buffer[char_total - 1] == 'A' && micli->buffer[char_total - 2] == '[' && micli->buffer[char_total - 3] == 27)
		// 	printf("\nFISTRO!! PECADOR DE LA PRADERA!!!!\n");
		// else if (micli->buffer[char_total - 1] == 'B' && micli->buffer[char_total - 2] == '[' && micli->buffer[char_total - 3] == 27)
		// 	printf("\nPOR LA GLORIA DE MI MADRE!!!!\n");
		// else if (micli->buffer[char_total - 1] == 'C' && micli->buffer[char_total - 2] == '[' && micli->buffer[char_total - 3] == 27)
		// 	printf("\nHASTA LUEGO LUCAS!!!!\n");
		// else if (micli->buffer[char_total - 1] == 'D' && micli->buffer[char_total - 2] == '[' && micli->buffer[char_total - 3] == 27)
		// 	printf("\nAL ATAQUERRRL!!!!\n");
		//printf("BYTES READ: %zu\n", char_total);

	}
}

char	micli_loop(t_micli *micli)
{
	while (1)
	{
		enable_raw_mode(&micli->raw_term, &micli->orig_term);
		signal(SIGINT, sigrun);
		//printf("🚀 ");
		write(STDOUT_FILENO, "🚀 ", 5);
		micli->active_line = micli_readline(micli);
		micli->active_line = clean_ft_memdup(micli->active_line, micli->cmdhist.active_line_size * sizeof(short), micli);
		push_to_hist_stack(micli, micli->active_line, &micli->cmdhist);
		//cmdhist_ptr_array_alloc(micli, &micli->cmdhist);
		if (micli->active_line[0] == 0)
			micli->buffer = ft_strdup("\0");
		else
			micli->buffer = ft_short_to_strdup(micli->active_line);
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
	ft_printf("\033[0;32m		               __       42Madrid\n");
	ft_printf("\033[0;32m		  __ _  (_)___/ (_) 	Mar 2021\n");
	ft_printf("\033[0;32m		 /  ' \\/ / __/ / /  	mrosario\n");
	ft_printf("\033[0;32m		/_/_/_/_/\\__/_/_/   	mvillaes\n\033[0m");
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
