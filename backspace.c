/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   backspace.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/29 03:03:22 by miki              #+#    #+#             */
/*   Updated: 2021/03/29 03:29:33 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This intimidating function is basically me asking ANSI to give me the
** the position of the cursor, which it encodes in an escape sequence that it
** sends to STDIN terminated in 'R' in the following format: ESC[xx;xxR, where
** 'xx' will be alphanumeric numbers. The first number refers to the rows and
** the second is cols. We are interested in the horizontal position, so we want
** the cols here.
**
** So what we have is a little parser (of course, this is minishell after all).
** After it finds the ';' it checks first to see if the next character is a 1
** and, if it is, then to see if the character after that is an 'R'. If both of
** those things are true, then the cursor position is indeed at col position 1,
** which is the left-most position on the screen. Otherwise, it is false, so the
** cursor could be at any other column.
**
** We are interested to know if the cursor is at column 1 because, if it is, the
** next character we will delete will be displayed in the right-most column of
** the line above where the cursor is, rather than in usual position behind the
** cursor. So we will use that information to make the backspace move the cursor
** accordingly.
**
** Not the prettiest implementation, or the most efficient, but functional. ;)
** What? I give you multiline backspace... and you COMPLAIN!? xD
*/

char	check_horizontal_cursor_pos(void)
{
	char	esc;

	esc = 0;
	write(STDOUT_FILENO, "\x1b[6n", 4);
	while (esc != 'R')
	{
		read(STDIN_FILENO, &esc, 1);
		if (esc == ';')
			if (read(STDIN_FILENO, &esc, 1) && esc == '1' && read(STDIN_FILENO, &esc, 1) && esc == 'R')
				return (1);
	}
	return (0);
}