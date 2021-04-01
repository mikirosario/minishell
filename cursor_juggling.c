/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cursor_juggling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/29 22:15:26 by miki              #+#    #+#             */
/*   Updated: 2021/04/01 20:04:53 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function moves the cursor up one row, executes carriage return to go to
** the first column of the row, calls get_window_size to retrieve the current
** number of columns in the window, which is saved to total_cols, and then moves
** the cursor right from the first column until the last column is reached.
**
** This ensures the cursor is not moved outside of the window space which, while
** some terminals can handle it, is undefined behaviour for other terminals.
**
** The net effect of this function is to move the cursor to the end of the
** preceding row, which I use to wrap around when deleting multi-row lines.
*/

void	wrap_up_right(t_micli *micli, t_termcaps *tcaps)
{
	unsigned short	total_cols;

	tputs(tcaps->cur_up, 1, pchr);
	tputs(tcaps->carriage_ret, 1, pchr);
	get_window_size(micli);
	total_cols = micli->winsize.ws_col;
	while (total_cols-- != 1)
		tputs(tcaps->cur_right, 1, pchr);
}

/*
** This function accepts a short directly from the buffer and converts it into
** a character string using my short_to_chars function. Then we proceed with a
** character insertion algorithm which I've put to together following GNU
** guidelines:
**
** https://www.gnu.org/software/termutils/manual/termcap-1.3/html_chapter/
** termcap_4.html#SEC32
**
** If the terminal we're using has an insert mode we activate it. If instead it
** has a special insert char command we need to input prior to inserting the
** caracter, we insert that. We insert the character. If the terminal uses
** padding, we insert the padding... I *THINK* this is done properly here?
** If the terminal has an exit_insert mode, we call it when we're done
** inserting. Then we free the character string.
*/

void	insert_char(t_termcaps *tcaps, short shortchr)
{
	char	*chr;

	chr = short_to_chars(shortchr);
	if (tcaps->inscaps.insert_mode)
		tputs(tcaps->inscaps.insert_mode, 1, pchr);
	else if (tcaps->inscaps.insert_char)
		tputs(tcaps->inscaps.insert_char, 1, pchr);
	tputs(chr, 1, pchr);
	if (tcaps->inscaps.insert_pad)
		tputs(tcaps->inscaps.insert_pad, 1, pchr);
	if (tcaps->inscaps.exit_insert)
		tputs(tcaps->inscaps.exit_insert, 1, pchr);
	chr = ft_del(chr);
}

/*
** This function deletes the character behind the cursor from the screen using
** termcaps commands. If the terminal has a delete mode, it is activated. The
** delete_char (dc) sequence is sent, which does... whatever it needs to in the
** terminal emulation. ;p
**
** It took me forever to figure out how this is done. No wonder bash developers
** just went for left-space-left. xD Termcaps documentation sucks! Thank
** goodness for open source code. :p
*/

void	del_from_screen(t_termcaps *tcaps)
{
	if (tcaps->delcaps.delete_mode)
		tputs(tcaps->delcaps.delete_mode, 1, pchr);
	tputs(tcaps->delcaps.delete_char, 1, pchr);
	if (tcaps->delcaps.delete_mode)
		tputs(tcaps->delcaps.exit_delete, 1, pchr);
}

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
**
** Addendum:
**
** I now use termcaps functions for this stuff (tputs) after rereading the
** subject and noticing the use of the word "OBGLIGATORY"... xD Termcaps
** basically is like C3P0, it speaks every terminal language and encodes
** commands like 'cursor up' in ways the local terminal will understand. For our
** terminals though, quite frankly, it is probably just generating the same
** VT100 codes I did manually... :p The del_line command requires the cursor
** to be at the left-most column according to its description, so that's why I
** do carriage return first for every line. I suppose that's because it uses
** \x1b[0K (clear line to right of cursor) instead of \x1b[2K (clear entire
** line) as I did. :p
**
** Some refs:
**
** https://www.gnu.org/software/termutils/manual/termcap-1.3/html_chapter/
** termcap_4.html
**
** https://www.gnu.org/software/termutils/manual/termcap-1.3/html_chapter/
** termcap_2.html
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
		tputs(micli->termcaps.carriage_ret, 1, pchr);
		tputs(micli->termcaps.del_line, 1, pchr);
		if (occupied_lines)
			tputs(micli->termcaps.cur_up, 1, pchr);
	}	
}
