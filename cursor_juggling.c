/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cursor_juggling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/29 22:15:26 by miki              #+#    #+#             */
/*   Updated: 2021/03/30 05:17:29 by miki             ###   ########.fr       */
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
			if (read(STDIN_FILENO, &esc, 1) && esc == '1' && \
			read(STDIN_FILENO, &esc, 1) && esc == 'R')
				return (1);
	}
	return (0);
}

/*
** This function will remove the previous line from the terminal window up to
** and including the prompt. For example, if, when this function is called, the
** terminal looks like this:
**
** | prompt> foo
** | prompt> foooooo
** | foooooooooooooo
**
** After it is called the terminal will look like this:
**
** | prompt> foo
** |
** |
**
** The function calls get_window_size to determine the column width of the
** terminal. That information, along with the total number of characters in the
** current prompt line + the number of characters in the prompt itself (5 in my
** case) is used to calculate how many rows must be occupied by the line by
** dividing the total number of characters plus the prompt by the total column
** width and adding 1 if there is any remainder. The result is stored in
** occupied_lines.
**
** The occupied_lines are all deleted successively and the cursor is moved up a
** row and to the right of the screen after every deletion except the last. On
** the last line, the cursor remains in the same row and at the left side of the
** screen.
**
** I don't bother to send an itoa of ws_col to move the cursor. I just assume
** the total number of columns will be something less than 9999 and let tty
** worry about where to stop. ;) That is because VT100 documentation defines
** that the behaviour when moving the cursor with C or D is that it will not be
** allowed past the edge of the screen.
**
** ANSI codes used are:
** \x1b[		== ESC_SEQ_START (2 bytes)
** 0123456789	== ARGUMENT (1 byte per digit)
** K			== ERASE IN LINE; for ARGUMENT == 2 clear entire line
** A			== CURSOR UP ARGUMENT rows; for no ARGUMENT, 1 row
** C			== CURSOR RIGHT ARGUMENT cols; for no ARGUMENT, 1 col
** \xd			== \r or CARRIAGE RETURN; I wrote it in C notation
**
** Like most ANSI-related functions, it's really much simpler than it looks. ;)
** https://en.wikipedia.org/wiki/ANSI_escape_code
*/

void	remove_prompt_line(t_micli *micli, short char_total)
{
	int	occupied_lines;

	get_window_size(micli);
	occupied_lines = (char_total + 5) / micli->winsize.ws_col;
	if ((char_total + 5) % micli->winsize.ws_col)
		occupied_lines += 1;
	while (occupied_lines--)
	{
		write(STDOUT_FILENO, "\x1b[2K\r", 5);
		if (occupied_lines)
			write(STDOUT_FILENO, "\x1b[A\x1b[9999C", 10);
	}	
}
