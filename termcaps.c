/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   termcaps.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 20:51:11 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/21 15:40:06 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


/*
** This function deletes num_bytes bytes from the buffer.
**
** The number of deleted bytes is returned. Use it to decrement the reported
** size
*/

size_t	del_from_buf(char *chr, size_t num_bytes)
{
	size_t deleted_bytes;

	deleted_bytes = num_bytes;
	while (num_bytes--)
		*chr-- = '\0';

	return (deleted_bytes);
}

/*
** This function returns 1 if the character is part of an escape sequence.
** Otherwise, 0 is returned. This function is controlled with a static char that
** serves as a flag called esc_seq.
**
** All escaped characters will be deleted from the buffer, and the size variable
** indicating occupied bytes in the buffer will be decremented accordingly.
**
** The flag esc_seq is set to 1 if an escape character is found, and to 2 if a
** '[' found after that.
**
** A character is part of an escape sequence if:
**
** 1. It's an unescaped escape, OR
** 2. The esc_seq flag is set.
**
** The esc_seq flag will be unset if the character after the escape is any
** character except a '[', though that character will still be considered
** escaped.
**
** If the character after an escape is a '[', the sequence will continue, and
** esc_bytes checked incremented to indicate that we have reached the third byte
** (esc_seq == 2) of the sequence the next time we enter the function.
**
** If the third byte of a sequence is reached we call is_arrow to check whether
** the sequence corresponds to one of the four arrow keys on the keyboard, which
** each send three-byte escape sequences as input (\x1b[A, \x1b[B, \x1b[C and
** \x1b[D for up, down, right and left, respectively), and act accordingly.
**
** YA QUE ESTAMOS VOY A PROHIBIR LAS TABULACIONES
*/

char	is_esc_seq(char *buf, size_t *size)
{
	char			*arrows;
	char			*arrow_addr;
	static char		esc_seq = 0;

	arrows = "ABCD";
	if (!esc_seq && buf[*size - 1] == '\x1b')
	{
		esc_seq++;
		// buf[*size - 1] = '\0';
		// *size -= 1;
		*size -= del_from_buf(&buf[*size - 1], 1);
		return (1);
	}
	else if (esc_seq == 1)
	{
		if (buf[*size - 1] == '[')
			esc_seq++;
		else
			esc_seq = 0;
		// buf[*size - 1] = '\0';
		// *size -= 1;
		*size -= del_from_buf(&buf[*size - 1], 1);
		return (1);
	}
	else if (esc_seq == 2)
	{
		arrow_addr = ft_strchr(arrows, buf[*size - 1]);
		if (arrow_addr)
		{
			if (arrow_addr < (arrows + 2))
				printf("BAILA MALDITO (CMD HIST)\n");
			else
				printf("NO PASHA NA TRONQUI (DESPLAZ FLECHA)\n");
			// buf[*size - 1] = '\0'; //delete chars from buf
			// buf[*size - 2] = '\0';
			// *size -= 2; //decrement size counter
		}
		*size -= del_from_buf(&buf[*size - 1], 1);
		esc_seq = 0;
		return (1);
	}
	return (0);
}


char	handle_esc_seq(char *main_buf, size_t *size)
{
	static char		sub_buf[2] = {0,0}; //resetea al quitar esc
	char			*arrows;
	char			*arrow_addr;
	static char		esc_bytes_checked = 0;

	arrows = "ABCD";
	if (!esc_bytes_checked)
	{
		sub_buf[0] = main_buf[*size - 1];
		esc_bytes_checked++;
	}
	else if (esc_bytes_checked == 1)
	{
		sub_buf[1] = main_buf[*size - 1];
		if (sub_buf[0] == '[')
		{
			arrow_addr = ft_strchr(arrows, sub_buf[1]);
			if (arrow_addr)
			{
				if (arrow_addr < (arrows + 2))
					printf("BAILA MALDITO (CMD HIST)\n");
				else
					printf("NO PASHA NA TRONQUI (DESPLAZ FLECHA)\n");
				main_buf[*size - 1] = '\0'; //delete chars from buf
				main_buf[*size - 2] = '\0';
				*size -= 2; //decrement size counter
			}
			else
				write(STDIN_FILENO, sub_buf, 2);
			esc_bytes_checked = 0;
			return (0);
		}
	}
	return (1);
}