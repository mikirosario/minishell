/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cntrl_char_actions.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 20:51:11 by mrosario          #+#    #+#             */
/*   Updated: 2021/04/01 14:33:23 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function determines whether a character being analysed by is_esc_seq is
** 'special'. Special characters are all deleted from the buffer. Some of them
** lead to specific actions while others will just be deleted and ignored.
**
** A character is special if:
**
** 1. It is part of an escape sequence, OR
** 2. It is any control character except '\n' or the control sequence ESC[,
** which happens to equate to the value 23323 in memory.
**
** If the character is special 1 will be returned, otherwise 0 will be returned.
*/

static int	is_special(short chr, char esc_seq)
{
	if (esc_seq || ((ft_iscntrl(chr) && chr != '\n') || chr == 23323))
		return (1);
	return (0);
}

/*
** Unescaped special characters will be handled as follows.
**
** //THIS DESCRIPTION DESCRIBED THE PRE-TERMCAPS VERSION; NOW OBSOLETE\\
** A DEL will cause the preceding character on the screen to be deleted by
** writing the sequence "ESC[D ESC[D" to STDOUT, which moves the cursor left one
** space, writes a space moving the cursor right, and moves the cursor left
** again, unless the cursor position is at column 1. If the cursor position is
** at column 1, it will move the cursor up a row and 9999 columns right using
** the ESC[C sequence to move the cursor to the right. VT100 manual states that
** if the C and D (but not H) are guaranteed to prevent the cursor from moving
** off the screen, so unless someone out there has a screen with 9999 columns
** this should bring us to the right-most column in the screen without bothering
** to measure it using ioctl - sorry, nothing against ioctl. ;p It will also
** cause the preceding character in the buffer to be nulled.
** \\THIS DESCRIPTION DESCRIBED THE PRE-TERMCAPS VERSION; NOW OBSOLETE//
**
** A DEL will now cause the preceding character on the screen to be deleted by
** writing a terminal-appropriate sequence to STDOUT as determined by the
** termcaps library. If the cursor is at column 1 (left-most column) then the
** wrap_up_right function is called to move it up and to the right-most column
** of the preceding line. Otherwise the cursor is moved left. In any case, the
** character at the new cursor position is then deleted. Since behaviour in case
** of moving past the edge of the screen is now undefined, as we won't
** necessarily be using vt100, wrap_up_right now moves the cursor to exactly the
** right-most column in the preceding row.
**
** An ESC ('x1b') will cause the escape sequence flag esc_seq to be set to 1 to
** indicate that the next character will be part of escape sequence phase 1.
**
** 23323 is the value of ESC[ in memory, and will cause the escape sequence flag
** esc_seq to be set to 2 to indicate that the  next character will be part of
** escape sequence phase 2.
*/

static void	handle_unescaped_special(short chr, short *buf, char *esc_seq, \
t_micli *micli)
{
	short	*char_total;
	char	cursor_pos;

	char_total = micli->cmdhist.char_total;
	if (chr == DEL && *char_total)
	{
		cursor_pos = check_horizontal_cursor_pos();
		if (cursor_pos == 1)
			wrap_up_right(micli, &micli->termcaps);
		else
			tputs(micli->termcaps.cursor_left, 1, pchr);
		del_from_screen(&micli->termcaps);
		*char_total -= del_from_buf(&buf[*char_total - 1], 1);
	}
	else if (chr == '\x1b')
		*esc_seq = 1;
	else if (chr == 20251 || chr == 23323)
		*esc_seq = 2;
}

/*
** In escape phase 1, a '[' character will cause the escape sequence to move to
** phase 2. any other character will reset the escape sequence to 0.
*/

static void	handle_esc_seq_phase_1(short chr, char *esc_seq)
{
	if (chr == '[' || chr == 'O')
		*esc_seq = 2;
	else
		*esc_seq = 0;
}

/*
** This function deletes num_bytes bytes from the buffer.
**
** The number of deleted bytes is returned. Use it to decrement the reported
** size
*/

size_t	del_from_buf(short *chr, size_t num_chars)
{
	size_t	deleted_chars;

	deleted_chars = num_chars;
	while (num_chars--)
		*chr-- = 0;
	return (deleted_chars);
}

/*
** Note: this function assumes that the CPU will be fast enough to process all
** instructions between one TTY keypress event and the next one before the
** default TTY VMIN expires. :) This is based on observed behaviour. Read on if
** that sounds interesting to you.
**
** //NOTES ON OBSERVED DEFAULT TTY/READ BEHAVIOUR\\
** This function depends on the following observed behaviour of TTY and read
** with UTF-8. If a key is pressed that sends a multi-byte character or an
** escape sequence to STDIN, it will come in fast enough that read will get
** two bytes of the sequence and put it in the provided short. So pressing 'ñ'
** for example will result in both bytes of 'ñ' being read at once and copied
** to the short. Pressing the up arrow will result in ESC[A being sent all at
** once, and ESC[ being copied to the short. This is called a burst and is
** controlled by the line-driver via VTIME, which will only allow read to return
** after VTIME has elapsed.
**
** If a key is held down, however, in principle TTY seems to have enough of a
** delay for sending data between discrete key-presses that it will be greater
** than VTIME. Thus, by default read will read one key-press BEFORE TTY sends
** the next one, so each byte of the repeated key-press is read to a separate
** short. Nice!
**
** The CPU also seems fast enough that after a key-press we will execute ALL the
** subsequent instructions and be back at read BEFORE TTY sends the next
** key press! So holding down 't' for example leads to [t][t][t][t] and not
** [tt][tt]. 
**
** That is quite convenient, and fortunate for me, because it saves me from
** having to tinker with the VTIME and VMIN settings of TTY and such, as the
** default settings seem to work fine for this application. ;p
**
** So. 3-byte or higher characters will print OK but won't delete properly.
** 4-byte escape sequences will just be ignored. 5-byte or longer escape
** sequences should be ignored up to the 4th byte and then printed out, but we
** don't use longer than 3-byte escape sequences with read, so... :p
**
** HOWEVER, you must be careful not to wait too long before the next read by
** bogging the CPU down with instructions or by introducing wait calls, or
** you'll 'miss the TTY boat', TTY will start sending characters to the STDIN
** buffer faster than they are being read, and characters will thus start to
** accumulate in the STDIN buffer.
**
** If that happens, then those characters would be read two-by-two and nothing
** would make sense anymore. xD I have experimented doing a whole bunch of
** system calls though and it seems that the default delay is long enough and
** the CPU fast enough to accommodate a *lot* of instructions. So it shouldn't
** be a problem on any modern CPU.
**
** Therefore, I do assume that the CPU will be fast enough to process all
** instructions between one TTY keypress event and the next one before the
** default TTY VTIME expires. :)
**
** //FUNCTION DESCRIPTION\\
** This function returns 1 if the character should not be echoed, or 0 if it
** should be echoed. We echo characters that are not 'special'. Sounds simple,
** but there are also some extra 'special things' to do with certain special
** characters that will complicate things. ;)
**
** All control characters are 'special' except \n, so they will be ignored. This
** includes 'tab'. Yay! Normal characters also become 'special' if escaped.
** Characters are escaped if 'esc_seq' is not zero. The 'esc_seq' variable will
** control for different characters at different parts of the sequence.
**
** All special characters will be deleted from the buffer, and the size variable
** indicating occupied bytes in the buffer will be decremented accordingly. The
** escape code can be written in ANSI as '\x1b', or in C as '\e', and is ASCII
** code 27. I'll use '\e' or ESC in the function description for clarity. I use
** '\x1b' in the code.
**
** Some special characters are more special than others and will do additional
** 'special things' besides just being deleted. xD The most special special
** characters are DEL (127) and the up and down arrows ('A' and 'B').
**
** DEL is a crazy guy who will not only be deleted itself, but also cause the
** character before it to be deleted. DEL (127) signals the backspace key has
** been pressed. It is a control character, so it is always special, but it will
** only be acted on if unescaped. If acted on, it will cause a sequence to be
** written to STDOUT that will move the cursor one space to the left, overwrite
** the character there with a space (which also moves the cursor back right),
** and then move the cursor left again so it appears after the last visible
** character. The overwritten character will be deleted from the buffer. DEL
** will be ignored if there are no more characters left in the buffer
** (*char_total == 0).
** 
** Not the most elegant way to do a backspace, I'm told, but for this old-school
** style project it will do nicely. :)
**
** 'A' and 'B' will cause minishell to scroll up and down the command history,
** which will change the active line to one of the lines in the history stack.
** 'A' and 'B' will only do this when found in escape sequence phase 2, however.
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
** shorts, this may happen in a single character (ESC[) or in two separate
** characters (ESC [). Both possibilities are accounted for.
**
** The esc_seq flag will be unset if the character in escape phase 1
** (esc_seq == 1) is any character except a '[' and will always be unset in
** escape phase 2 (esc_seq == 2), though that character will still be considered
** special/escaped. That is to say, it will always be unset at the character
** after the '[', which is not proper, of course, but proper enough to get the
** arrow keys working, which is what is needed for this assignment. ;)
**
** If the character after an escape is a '[' (or the initial character was a
** 'ESC[' two-byte combination), the sequence will continue, and the esc_seq
** flag incremented accordingly to indicate that we should now check the second
** (esc_seq == 1) or third (esc_seq == 2) byte of the sequence, as appropriate,
** the next time we enter the function.
**
** If the third byte of a sequence is reached we call is_arrow to check whether
** the sequence corresponds to the up or down arrow keys on the keyboard. The
** arrow keys, when pressed, each send three-byte escape sequences as input
** (ESC[A, ESC[B, ESC[C and ESC[D for up, down, right and left, respectively).
** If they are up or down arrows then the scroll flag will be set accordingly to
** 1 or -1.
**
** SPECIAL NOTE: This function was written before I added the two data shorts to
** the beginning of each line, which is why it adresses char_total - 1 as the
** last character. Rather than change the function I now sent it the address
** &hist_stack[index][2] as buf, so it continues to work as if the first two
** data shorts didn't exist. Sorry if it's confusing. :p
**
** Insert this code at the beginning to display character codes:
** 	//DEBUG CODE DISPLAY
**	printf("%hd\n", chr);
**	//DEBUG CODE DISPLAY
*/

char	do_not_echo(short *buf, short *char_total, char *scroll_flag, \
t_micli *micli)
{
	static char		esc_seq = 0;
	short			chr;

	chr = buf[*char_total - 1];
	if (is_special(chr, esc_seq))
	{
		*char_total -= del_from_buf(&buf[*char_total - 1], 1);
		if (!esc_seq)
			handle_unescaped_special(chr, buf, &esc_seq, micli);
		else if (esc_seq == 1)
			handle_esc_seq_phase_1(chr, &esc_seq);
		else if (esc_seq == 2)
		{
			esc_seq = 0;
			if (chr == 'A')
				*scroll_flag = 1;
			else if (chr == 'B')
				*scroll_flag = -1;
		}
		return (1);
	}
	return (0);
}
