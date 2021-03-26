/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   termcaps.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 20:51:11 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/26 09:07:42 by miki             ###   ########.fr       */
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
	/////////////////////
** NOTE: In Linux, if I print debug text after an arrow check, and hold the
** arrow key down, any arrow sequences subsequent to the first sequence come in
** a cycling order of ESC[ -> DESC -> [D. If I don't, however, they seem to
** remain in order ESC[ -> D -> ESC[. O_O That's convenient, but I don't know
** why it's happening, and I don't know if I quite trust it. The same thing
** happens with tab. If I uncomment the debug text and hold down tab, tabs come
** in two at a time. If I comment it, they come in neatly one at a time. Need to
** test this in Mac!
** 	////////////////////
**
*/

char	is_esc_seq(short *buf, short *char_total, char *move_flag)
{
	static char		esc_seq = 0;
	short			start_seq;
	short			chr;

	//'\x1b[' == 23323
	start_seq = 23323;
	chr = buf[*char_total - 1];

	// //DEBUG CODE DISPLAY
	// printf("%hd\n", chr);
	// //DEBUG CODE DISPLAY

	//CASE 0
	if (!esc_seq && (chr == '\t' || chr == 2313)) //2313 is the decimal representation of the value in chr with two contiguous tabs; happens when you hold down the tab key (NOT ALWAYS!!!! ONLY WITH MY DEBUG CODE!!!! O_O TRUE ALSO IN MAC?????)
	{
		*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		// //DEBUG CODE
		// write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
		// write(STDOUT_FILENO, "BAD BUNNY! NO TAB!", 18);
		// if (chr == '\t') //only sleep with one tab to limit accumulated sleep times when holding tab :p
		// 	sleep(1);
		// write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
		// write(STDOUT_FILENO, "🚀 ", 5);
		// write(STDOUT_FILENO, buf, *char_total * sizeof(short));
		// //DEBUG CODE
		
		return(1);
	}
	//CASE 1
	else if (!esc_seq && chr == DEL) //unescaped backspace
	{
		*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		if (*char_total)
		{
			write(STDOUT_FILENO, "\x1b[D \x1b[D", 7);
			*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		}
		return (1);
	}
	//CASE 2
	else if (!esc_seq && chr == '\x1b') //unescaped lone esc
	{
		esc_seq = 1;
		*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		return (1);
	}
	//CASE 3
	else if (!esc_seq && chr == start_seq) //unescaped esc sequence \x1b[
	{
		esc_seq = 2; //set sequence id
		// buf[*char_total - 1] = '\0';
		// *char_total -= 1;
		*char_total -= del_from_buf(&buf[*char_total - 1], 1); //delete char (note, number indicates CHAR NOT BYTES! one deleted char is always a short/two bytes)
		return (1); //return -> functionalize all this
	}
	else if (esc_seq == 1)
	{
		//CASE 3
		*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		if (chr == '[')
			esc_seq = 2;
		else
			esc_seq = 0;
		if (chr == 16731 || chr == 16987 || chr == 17243 || chr == 17499)
		{
			if (chr == 16731)
				*move_flag = 1;
			else if (chr == 16987)
				*move_flag = -1;
			else if (chr == 17243)
			{
				// //DEBUG CODE
				// write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
				// write(STDOUT_FILENO, "FISTRO!! PECADOR DE LA PRADERA!!!!", 34);
				// sleep(1);
				// write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
				// write(STDOUT_FILENO, "🚀 ", 5);
				// write(STDOUT_FILENO, buf, *char_total * sizeof(short));
				// //DEBUG CODE
			}
			else if (chr == 17499)
			{
				// //DEBUG CODE
				// write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
				// write(STDOUT_FILENO, "POR LA GLORIA DE MI MADRE!!!!", 29);
				// sleep(1);
				// write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
				// write(STDOUT_FILENO, "🚀 ", 5);
				// write(STDOUT_FILENO, buf, *char_total * sizeof(short));
				// //DEBUG CODE
			}
		}
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
	//CASE 4
	else if (esc_seq == 2)
	{
		*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		esc_seq = 0; 
		if (chr == 6977 || chr == 6978 || chr == 6979 || chr == 6980)
			esc_seq = 1;
		//arrow_addr = ft_strchr(arrows, (char)buf[*char_total - 1]);
		//if (buf[*char_total - 1] >= 'A' && buf[*char_total - 1] <= 'D')

		if (chr == 'A' || chr == 6977)
			*move_flag = 1;
		else if (chr == 'B' || chr == 6978)
			*move_flag = -1;
		else if (chr == 'C' || chr == 6979)
		{
			// //DEBUG CODE
			// write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
			// write(STDOUT_FILENO, "FISTRO!! PECADOR DE LA PRADERA!!!!", 34);
			// sleep(1);
			// write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
			// write(STDOUT_FILENO, "🚀 ", 5);
			// write(STDOUT_FILENO, buf, *char_total * sizeof(short));
			// //DEBUG CODE
		}
		else if (chr == 'D' || chr == 6980)
		{
			// //DEBUG CODE
			// write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
			// write(STDOUT_FILENO, "POR LA GLORIA DE MI MADRE!!!!", 29);
			// sleep(1);
			// write(STDOUT_FILENO, "\x1b[2K\r", 5); //\x1b[2K == erase line, \r == carriage return in ANSI-speak
			// write(STDOUT_FILENO, "🚀 ", 5);
			// write(STDOUT_FILENO, buf, *char_total * sizeof(short));
			// //DEBUG CODE
		}
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