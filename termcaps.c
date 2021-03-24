/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   termcaps.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 20:51:11 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/24 21:09:56 by miki             ###   ########.fr       */
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
** This function returns 1 if the character is part of an escape sequence, or if
** it signals the backspace key has been pressed. Otherwise, 0 is returned. This
** function is controlled with a static char that serves as a flag called
** esc_seq. I'll explain the backspace at the end.
**
** All escaped characters will be deleted from the buffer, and the size variable
** indicating occupied bytes in the buffer will be decremented accordingly. The
** escape code can be written in ANSI as '\x1b', or in C as '\e', and is ASCII
** code 27. I'll use '\e' in the function description for clarity. I use '\x1b'
** in the code.
**
** This function can potentially handle sequences of up to 4 bytes. Larger
** sequences may come in corrupted as the initial part of the sequence will be
** deleted and subsequent sequence characters will be treated normally. It can
** be easily expanded to handle more bytes in a sequence, however.
**
** 42 school doesn't allow us to use switch, so that's why this is all done with
** ifs and elses, if you're wondering. :p
**
** The flag esc_seq is set to 1 if an escape character is found, and to 2 if a
** '[' is found after that. Note that since the character buffer is a buffer of
** shorts, this may happen in a single character or in two separate characters.
** Both possibilities are accounted for.
**
** A character is part of an escape sequence if:
**
** 1. It's an unescaped escape, OR
** 2. The esc_seq flag is set.
**
** The esc_seq flag will be unset if the character after the escape is any
** character except a '[', though that character will still be considered
** escaped. It will always be unset at the character after the '[', which is not
** proper, of course, but proper enough to get the arrow keys working, which is
** what is needed for this assignment. ;)
**
** If the character after an escape is a '[' (or the initial character was a
** '\e[' two-byte combination), the sequence will continue, and the esc_seq
** flag incremented accordingly to indicate that we should now check the second
** (esc_seq == 1) or third (esc_seq == 2) byte of the sequence, as appropriate,
** the next time we enter the function.
**
** If the third byte of a sequence is reached we call is_arrow to check whether
** the sequence corresponds to one of the four arrow keys on the keyboard, which
** each send three-byte escape sequences as input (\x1b[A, \x1b[B, \x1b[C and
** \x1b[D for up, down, right and left, respectively), and act accordingly.
**
** You'll notice that at the initial if handles the backspace key. The backspace
** key inputs a character that fits in a single byte (the low byte of a short)
** and is equal to 127 in ASCII. The constant DEL is defined to 127 in
** minishell.h. In the event a DEL character has been sent, we write "\e[D \e[D"
** to terminal (stdout). That is left, space, left. This will cause the cursor
** to move one space to the left, overwrite whatever character was displayed
** there with a space (which also moves the cursor right again), and then move
** left again to leave the cursor in front of the preceding character. Not the
** most elegant way to do a backspace, I'm told, but for this old-school style
** project it will do nicely. :)
**
** YA QUE ESTAMOS VOY A PROHIBIR LAS TABULACIONES
*/

char	is_esc_seq(short *buf, size_t *char_total, char *move_flag)
{
	static char		esc_seq = 0;
	short			start_seq;

	//'\x1b['
	start_seq = 23323;
	if (!esc_seq && buf[*char_total - 1] == '\t')
	{
		*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		
		//DEBUG CODE
		write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
		write(STDOUT_FILENO, "BAD BUNNY! NO TAB!", 18);
		sleep(1);
		write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
		write(STDOUT_FILENO, "🚀 ", 5);
		write(STDOUT_FILENO, buf, *char_total * sizeof(short));
		//DEBUG CODE
		
		return(1);
	}
	else if (!esc_seq && buf[*char_total - 1] == DEL) //unescaped backspace
	{
		*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		if (*char_total)
		{
			write(STDOUT_FILENO, "\x1b[D \x1b[D", 7);
			*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		}
		return (1);
	}
	else if (!esc_seq && buf[*char_total - 1] == start_seq) //unescaped esc sequence \x1b[
	{
		esc_seq = 2; //set sequence id
		// buf[*char_total - 1] = '\0';
		// *char_total -= 1;
		*char_total -= del_from_buf(&buf[*char_total - 1], 1); //delete char (note, number indicates CHAR NOT BYTES! one deleted char is always a short/two bytes)
		return (1); //return -> functionalize all this
	}
	else if (!esc_seq && buf[*char_total - 1] == '\x1b') //unescaped lone esc
	{
		esc_seq = 1;
		*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		return (1);
	}
	else if (esc_seq == 1)
	{
		if (buf[*char_total - 1] == '[')
			esc_seq = 2;
		else
			esc_seq = 0;
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
	else if (esc_seq == 2)
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