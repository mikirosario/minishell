/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   termcaps.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 20:51:11 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/23 21:43:20 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


/*
** This function deletes num_bytes bytes from the buffer.
**
** The number of deleted bytes is returned. Use it to decrement the reported
** size
*/

size_t	del_from_buf(short *chr, size_t num_chars)
{
	size_t deleted_chars;

	deleted_chars = num_chars;
	while (num_chars--)
		*chr-- = 0;

	return (deleted_chars);
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

char	is_esc_seq(short *buf, size_t *char_total, char *move_flag)
{
	static char		esc_seq = 0;
	short			start_seq;

	//'\x1b['
	start_seq = 23323;
	if (!esc_seq && buf[*char_total - 1] == DEL)
	{
		*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		if (*char_total)
		{
			write(STDOUT_FILENO, "\x1b[D \x1b[D", 7);
			*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		}
		return (1);
	}
	else if (!esc_seq && buf[*char_total - 1] == start_seq)
	{
		esc_seq++;
		// buf[*char_total - 1] = '\0';
		// *char_total -= 1;
		*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		return (1);
	}
	// else if (esc_seq == 1)
	// {
	// 	if (buf[*char_total - 1] == '[')
	// 		esc_seq++;
	// 	else
	// 		esc_seq = 0;
	// 	// buf[*char_total - 1] = '\0';
	// 	// *char_total -= 1;
	// 	*char_total -= del_from_buf(&buf[*char_total - 1], 1);
	// 	return (1);
	// }
	else if (esc_seq == 1)
	{
		//arrow_addr = ft_strchr(arrows, (char)buf[*char_total - 1]);
		if (buf[*char_total - 1] >= 'A' && buf[*char_total - 1] <= 'D')
		{
			{
				if (buf[*char_total - 1] == 'A')
					*move_flag = 1;
				else if (buf[*char_total - 1 == 'B'])
					*move_flag = -1;
			}
		}
		
		*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		esc_seq = 0;
		return (1);
	}
	return (0);
}


	// else if (esc_seq == 2)
	// {
	// 	arrow_addr = ft_strchr(arrows, buf[*char_total - 1]);
	// 	if (arrow_addr)
	// 	{
	// 		if (arrow_addr < (arrows + 2)) //si arrow_addr > arrows + 2 son flechas derecha e izquierda... no  hacemos nada con ellas en esta versión
	// 		{
	// 			//printf("BAILA MALDITO (CMD HIST)");
	// 			if (*arrow_addr == 'A')
	// 				*move_flag = 1;
	// 			else
	// 				*move_flag = -1;
	// 		}
	// 		//esc_seq = 0;
	// 	}
	// 	// else if (buf[*char_total - 1] == '3')
	// 	// 	esc_seq++;
	// 	//else
	// 	esc_seq = 0;
	// 	*char_total -= del_from_buf(&buf[*char_total - 1], 1);
	// 	return (1);
	// }
	// // else if (esc_seq == 3)
	// // {
	// // 	if (buf[*char_total - 1] == '~')
	// // 	{
	// // 		write(STDOUT_FILENO, "\x1b[D \x1b[D", 3);
	// // 	}
	// // 	*char_total -= del_from_buf(&buf[*char_total - 1], 1);
	// // }
	// return (0);