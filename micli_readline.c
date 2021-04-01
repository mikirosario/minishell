/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   micli_readline.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/30 01:08:17 by miki              #+#    #+#             */
/*   Updated: 2021/04/01 11:21:45 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function reserves memory for an empty scratch log to be used as the
** readline buffer, where all text input by the user into the terminal is stored
** for editing. It also initializes all the necessary parameters to manage the
** scratch log. The scratch log is always set as the penultimate member of the
** hist_stack array of short strings (the last member is a null terminator). So
** the index position of the scratch log in the hist_stack array is always
** ptrs_in_hist - 1, bearing in mind that the null terminator is now counter in
** ptrs_in_hist.
**
** At the beginning, the memory allocated for the readline buffer is
** READLINE_BUFSIZE + 3 reserved memory slots. The READLINE_BUFSIZE is for
** editing text. Two shorts at the beginning of the line are for storing the
** total number of characters present in the line at any given moment, and the
** line buffer size. The convenience pointers cmdhist->char_total and
** cmdhist_bufsize are pointed directly to these addresses, and will be kept
** up to date if they change. The third reserved memory slot is for the null
** termination.
**
** Text will be written to this buffer one short at a time as it is read from
** STDIN by the read function from within the micli_readline function. Here is
** a hist_stack example where the READLINE_BUFSIZE is 10:
**
** 0 Oldest Entry	-> "16,20,echo Hello World"
** 1 Later Entry	-> "9,10,ls myflie"
** 2 Newest Entry	-> "9,10,ls myfile"
** 3 Scratch Log	-> "0,10,"
** 4 Null Ptr		-> 0
**
** The char_total pointer points to the first short in the line buffer. The
** bufsize pointer points to the second short in the line buffer. The *bufsize
** value is intiialized to READLINE_BUFSIZE. The char_total value is initialized
** to 0.
**
** The bufsize variable is explicitly initialized as READLINE_BUFSIZE. The
** char_total variable is implicitly initialized to 0, because we use calloc,
** which zeroes all reserved memory.
*/

void	scratch_log_init(t_micli *micli, t_cmdhist *cmdhist, short **hist_stack)
{
	cmdhist->index = cmdhist->ptrs_in_hist - 1;
	hist_stack[cmdhist->index] = \
	clean_calloc(READLINE_BUFSIZE + 3, sizeof(short), micli);
	cmdhist->char_total = &hist_stack[cmdhist->index][0];
	cmdhist->bufsize = &hist_stack[cmdhist->index][1];
	*cmdhist->bufsize = READLINE_BUFSIZE;
}

/*
** This function decides whether or not we must heed the user's scroll request
** or ignore, depending on whether any more lines of history exist in the
** requested scroll direction. To understand how this works, a brief description
** of how the hist_stack looks is necessary.
**
** The oldest entries are at the lowest addresses. Thus, scrolling up means
** decrementing the index and scrolling down means incrementing it. We only want
** to scroll up if we are not at the beginning of the list, and we only want to
** scroll down if we are not at the end of it (which is the scratch log). The
** variable ptrs_in_hist tracks the number of pointers in the history stack, not
** including the null.
**
** So, to visualize all this, a history stack starts out like this:
**
** ptrs_in_hist	: 1
**
** 0 Scratch Log	-> ""
** 1 Null Ptr		-> 0
**
** And may end up like this
**
** ptrs_in_hist	: 4
**
** 0 Oldest Entry	-> "echo Hello World"
** 1 Later Entry	-> "ls myflie"
** 2 Newest Entry	-> "ls myfile"
** 3 Scratch Log	-> ""
** 4 Null Ptr		-> 0
**
** Thus, ptrs_in_hist - 1, translating number of pointers to position, gets us
** to the scratch log. The variable cmdhist->index is initialized to
** ptrs_in_hist - 1, so it addresses the scratch log by default whenever the
** hist_stack is updated with a new entry.
**
** So, when cmdhist->scroll == 1 (up arrow), we only allow scrolling up if the
** index is greater than 0. This is because if cmdhist->index is already 0, then
** we are already at the Oldest Entry and there is nowhere left to scroll up.
** Likewise, when cmdhist->scroll == -1 we only allow scrolling down if the
** cmdhist->index is greater than ptrs_in_hist - 1. This is because if
** cmdhist->index == ptrs_in_hist - 1, we we are already at the most recent
** possible entry (the scratch log) and so there is nowhere left to scroll down.
*/

int	must_scroll(t_cmdhist *cmdhist)
{
	if ((cmdhist->scroll == 1 && cmdhist->index > 0) \
	 || (cmdhist->scroll == -1 && cmdhist->index < cmdhist->ptrs_in_hist - 1))
		return (1);
	return (0);
}

/*
** This function reacts to scroll requests, and will change the active line to
** an older or newer entry in the command history depending on whether the up
** or down arrows are pressed.
**
** If an up or down arrow key is pressed it will set the scroll flag to 1 or -1,
** respectively. A check will be done to ensure that there are more entries in
** the history in the requested direction of scrolling. If there are, we enter
** this function to execute the request.
**
** The hist_stack is made up of a history of lines of text typed into the
** command line in the past and sent for parsing with the enter key. At the end
** of the stack there is a line used as a 'scratch' log, which is where we write
** the newest line, and after that there is a null pointer to terminate.
**
** The oldest entries are at the lowest addresses. Thus, scrolling up means
** decrementing the index and scrolling down means incrementing it. We only want
** to scroll up if we are not at the beginning of the list, and we only want to
** scroll down if we are not at the end of it (which is the scratch log). The
** variable ptrs_in_hist tracks the number of pointers in the history stack, not
** including the null.
**
** So, to visualize all this, a history stack starts out like this:
**
** ptrs_in_hist	: 1
**
** 0 Scratch Log	-> ""
** 1 Null Ptr		-> 0
**
** And may end up like this
**
** ptrs_in_hist	: 4
**
** 0 Oldest Entry	-> "echo Hello World"
** 1 Later Entry	-> "ls myflie"
** 2 Newest Entry	-> "ls myfile"
** 3 Scratch Log	-> ""
** 4 Null Ptr		-> 0
**
** Thus, ptrs_in_hist - 1, translating number of pointers to position, gets us
** to the scratch log. The variable cmdhist->index is initialized to
** ptrs_in_hist - 1, so it addresses the scratch log by default whenever the
** hist_stack is updated with a new entry.
**
** To do the scroll, first we increment or decrement the cmdhist->index, as
** appropriate. Then we call remove_prompt_line, which handily deletes the
** preceding prompt and any associated lines of text for us to make space for
** the line we are going to print from history. Check its function description
** for the gory details of how *that* works. ;) Then we make sure to reset our
** friendly neighbourhood pointers char_total and bufsize to point to the
** relevant char_total and bufsize of this history line. Then we just write out
** the normal prompt, and write out the text in the history line, which will,
** of course, consist of char_total * sizeof(short) bytes.
*/

void	do_scroll(t_micli *micli, t_cmdhist *cmdhist, short **hist_stack)
{
	if (cmdhist->scroll == 1)
		cmdhist->index--;
	if (cmdhist->scroll == -1)
		cmdhist->index++;
	remove_prompt_line(micli, *cmdhist->char_total);
	cmdhist->char_total = &hist_stack[cmdhist->index][0];
	cmdhist->bufsize = &hist_stack[cmdhist->index][1];
	write(STDOUT_FILENO, "🚀 ", 5);
	write(STDOUT_FILENO, &hist_stack[cmdhist->index][2], \
	*cmdhist->char_total * sizeof(short));
}

/*
** This function is called by micli_readline when a newline is sent by the user
** (enter key). After this function is completed, our command line will be sent
** out for parsing.
**
** Our first task here is to delete the \n from the buffer, transforming it into
** a null.
**
** Now we are going to send this command line to be parsed, but first we will
** want it to be copied into the command history by pushing it to the command
** history stack. To do that without hogging memory unnecessarily, we need to
** recalculate the minimum buffer size necessary (as a multiple of
** READLINE_BUFSIZE) to hold all of the remaining characters in the buffer +
** one more free character space for read to put another character into in the
** future. We can derive that figure by dividing the READLINE_BUFSIZE by the
** number of characters in the line (char_total) + 1 for the extra space, then
** adding READLINE_BUFSIZE to the numerator and dividing again until the result
** is NOT 0.
**
** We save the result to micli->recalc_bufsize and return the address of our
** active line for processing... Remember, bufsize refers to total number of
** characters that can fit in the line, not including the data segments and the
** null.
**
** The address of the line will be onforwarded to the main loop by
** micli_readline, where it will be copied, pushed to the hist_stack, converted
** into a character string and eventually freed from this mortal coil.
**
** Note how, if we DIDN'T receive a \n here and so DIDN'T delete the new
** character from the buffer, our buffer might now be full... that's why we
** check whether we need to reallocate AFTER the newline check. :)
*/

short	*send_to_parser(t_cmdhist *cmdhist, short **hist_stack)
{
	*cmdhist->char_total -= \
	del_from_buf(&hist_stack[cmdhist->index][*cmdhist->char_total + 1], 1);
	cmdhist->recalc_bufsize = READLINE_BUFSIZE;
	while (cmdhist->recalc_bufsize / (*cmdhist->char_total + 1) == 0)
		cmdhist->recalc_bufsize += READLINE_BUFSIZE;
	return (hist_stack[cmdhist->index]);
}

/*
** First we use scratch_log_init to set up a readline_buffer that will be used
** to record characters from read as they are read from STDIN. The initial
** readline_buffer is called the scratch log and is part of the hist_stack short
** array, which will store all past command line entries and can be scrolled
** through using the arrow keys.
**
** The first task in the while is a scroll check to see if we need to
** change the active line to one of the lines in our history (hist_stack). If we
** we do need to scroll through the history, we call the do_scroll function and
** it does it for us. Check its function description for the gritty details.
** After the cmdhist->scroll check we always reset the scroll flag to 0. It
** needs to be reset here because we might have a scroll flag set but not allow
** a scroll, because we're at the beginning or end of the history stack!
**
** The next task is the read function.
**
** In canonical mode, when reading from STDIN the read() function hangs until
** '\n' newline or terminal EOF is input, after which it will read the first
** thing in its buffer in a chunk defined by the third argument and copy it to
** the address passed as the second argument. If the third argument defines a
** chunk size that is larger than what remains in the buffer to be read, the
** smaller size will be read. For every subsequent call it will read the
** subsequent chunk in the buffer until the newline or EOF. Or... so I thought
** when I started this project!
**
** The reality, though, is that READ DOESN'T ACTUALLY WAIT FOR THE NEW LINE AT
** ALL - that is a LIE your parents told you. What actually happens is the line
** discipline of tty withholds data from STDIN until IT receives a newline or
** EOF, so read is actually hanging for the same reason it always hangs - no
** data coming through the pipe. ;) The tty is manipulating read to take the
** data in line by line. This is part of canonical mode, which also does some
** rudimentary pre-processing and interpreting of keys.
**
** As canonical mode has been disabled to enable us to interpret the arrow keys
** as scroll controls, the tty will no longer wait until it receives a newline
** before sending key data into STDIN to be read, but will rather send them
** basically as soon as it receives them (technically, it will make VMIN bytes
** of data available as soon as VTIME has passed since the last key press, and
** by default, those are set at 1 byte and 1 tenth of a second, as I understand
** it). This means we must now individually process every key press ourselves,
** and read will be passing them along as soon as they are typed in.
**
** If we don't want a key to remain in the buffer after processing, we use
** del_from_buf to remove it cleanly. UTF-8 mode is on and so characters may be
** passed as shorts (2-bytes). We read all characters, whether single-byte or
** double-byte characters, into a string of shorts so that common two-byte
** multichars, like 'ñ', will be deleted correctly.
**
** As mentioned before, each line in the history stack begins with four bytes
** (two shorts) that contain data about the line. The first short contains a
** number indicating the total number of characters present in the line, and we
** save its address as char_total. The second two contain a number indicating
** total number of characters that will fit in the line, and we save its address
** as bufsize. Note, bufsize does not include the first two shorts used to store
** data, or the last one used as a null terminator, thus, WE ALWAYS RESERVE
** (BUFSIZE + 3) * SIZEOF(SHORT) BYTES TO MAKE A LINE.
**
** The program must return to the read functon after reading a character before
** VTIME has passed or characters may start to accumulate in the buffer. Since
** we are reading the characters in two-byte increments, this can corrupt
** continguous single byte characters, as they would be read as a single
** two-byte character, which would cause backspace to delete two characters
** from the buffer but only show one of them disappearing. Short of using
** sleep(1), though, I haven't been able to provoke this just by introducing
** more instructions, so we should be safe. ;)
**
** While in canonical mode, EOF will be returned by (somehow, I don't know how
** it does this xD) getting read to return 0. In non-canonical mode, however,
** EOF is simply '4'. I turn canonical mode back on once lines are sent in for
** parsing, and I don't turn it off again until commands are executed and the
** STDIN buffer is read, so we need to account for both size == 0 and
** chr_read == 4 as potentially signalling EOF.
**
** Because there are two data segments before the string proper begins, we will
** always want to read to hist_stack[cmdhist->index][*cmdhist->char_total + 2].
** As a corollary, since we increment char_total as soon as we read a character
** into the buffer, we will always want to address
** hist_stack[cmdhist->index][*cmdhist->char_total + 1] for the most recent
** written character. See the model below to visualize why:
**
**						BEFORE READ
**		CHRTOTAL   BUFSIZ	CHR		CHR		NULL
** short  [0]		[1]		[2]		[3]		[4]
** value:  1		 2		'C'		 0		 0
**		 1 + 2						 ↑
**						AFTER READ
**		CHRTOTAL   BUFSIZ	CHR		CHR		NULL
** short  [0]		[1]		[2]		[3]		[4]
** value:  2		 2		'C'		'C'		 0	
**		 2 + 1						 ↑
**
** When characters are read into the buffer, the first thing we do is make a
** note of it by incrementing char_total. We will likewise decrement char_total
** whenever we delete a character from the buffer.
**
** The next thing we do is check for an escape sequence or special character and
** process accordingly. If the character is NOT special, we first need to "echo"
** it back to the user, meaning, print it on the screen. Canonical also used to
** do this for us, but not since we turned it off. ;) So we do that.
**
** If a newline is sent, we are going to send this command line to be parsed.
** First, however, we call send_to_parser to perform some additional vital tasks
** before we hand the reins over to the parser...
**
** If the character is not a new line, what we need to do next is check to see
** if we need to reallocate the buffer, because... yes children, this is a
** dynamically allocated buffer! ;) We start out with room for READLINE_BUFSIZE
** characters, and, if we need more space, we add another READLINE_BUFSIZE and
** reallocate to memory of that size. And we remember to reset the char_total
** and bufsize pointers to the new memory location, too. ;)
**
** WHERE DOES IT END? You may ask. Surely we cannot simply keep asking for
** memory unto infinity! Alas, you are right. It ends at SHRT_MAX. This is
** because the memory address where we store bufsize is a short, so we literally
** cannot fit any number higher than SHRT_MAX there. xD This leaves us room for
** about 32000 or so characters in a single line... surely that's enough for
** anyone? Surely? O_O Well, perhaps not my evaluator, hence the hard cap...
**
** If you reach SHRT_MAX - 1 characters in a line and try to type more, micli
** deletes your character scolds you furiously for your impudence.
**
** This was such an unassuming little function, too, before termcaps became
** mandatory... xD
*/

short	*micli_readline(t_micli *micli, t_cmdhist *cmdhist, short **hist_stack)
{
	ssize_t	size;

	scratch_log_init(micli, cmdhist, hist_stack);
	while (1)
	{
		if (must_scroll(cmdhist))
			do_scroll(micli, cmdhist, hist_stack);
		cmdhist->scroll = 0;
		size = read(STDIN_FILENO, \
		&hist_stack[cmdhist->index][*cmdhist->char_total + 2], 2);
		if (hist_stack[cmdhist->index][*cmdhist->char_total + 2] == 4 || !size)
		{
			write(1, "exit\n", 5);
			exit_success(micli);
		}
		*cmdhist->char_total += 1;
		if (!do_not_echo(&hist_stack[cmdhist->index][2], \
		cmdhist->char_total, &cmdhist->scroll, micli))
		{
			insert_char(&micli->termcaps, \
			hist_stack[cmdhist->index][*cmdhist->char_total + 1]);
			if (hist_stack[cmdhist->index][*cmdhist->char_total + 1] == '\n')
				return (send_to_parser(cmdhist, hist_stack));
		}
		if (*cmdhist->char_total == *cmdhist->bufsize)
			reallocate_readline_buffer(micli, cmdhist, hist_stack);
	}
}
