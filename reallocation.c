/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reallocation.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/30 03:34:16 by miki              #+#    #+#             */
/*   Updated: 2021/04/02 04:15:08 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function will reallocate the readline buffer to accomodate an additional
** READLINE_BUFSIZE characters... or die trying.
**
** We use clean_realloc to do the reallocation, which, in turn, uses ft_realloc.
** We increment bufsize by READLINE_BUFSIZE and take bufsize + 3 * sizeof(short)
** as our new memory size to make space for bufsize character + 3 reserved
** shorts: two initial data segments to store char_total and bufsize, and one
** null terminator. We pass char_total + 2 as our old memory size that we want
** copied: that is, all of the characters in our buffer plus our two initial
** data segments recording char_total and bufsize.
**
** The pointers to char_total and bufsize need to be updated every time the
** buffer is reallocated, so we do that here too.
**
** If the line buffer size is equal to SHRT_MAX, we do not allow the realloc to
** happen. Instead, the function will decide that it has had enough shenanigans,
** delete the last character from the buffer and print an embarrassing error
** message for the user. For shame!
*/

void	reallocate_readline_buffer(t_micli *micli, t_cmdhist *cmdhist, \
short **hist_stack)
{
	if (*cmdhist->bufsize < SHRT_MAX)
	{
		*cmdhist->bufsize += READLINE_BUFSIZE;
		hist_stack[cmdhist->index] = clean_realloc(hist_stack[cmdhist->index], \
		(*cmdhist->bufsize + 3) * sizeof(short), \
		(*cmdhist->char_total + 2) * sizeof(short), micli);
		cmdhist->char_total = &hist_stack[cmdhist->index][0];
		cmdhist->bufsize = &hist_stack[cmdhist->index][1];
	}
	else
	{
		
		del_from_screen(&micli->termcaps);
		//write(STDOUT_FILENO, "\x1b[D \x1b[D\a", 8);
		*cmdhist->char_total -= del_from_buf(&hist_stack[cmdhist->index][micli->termcaps.curpos_buf], 1);
		//del_from_buf(&hist_stack[cmdhist->index][*cmdhist->char_total + 1], 1);
		//write(STDOUT_FILENO, BUF_OVERFLOW, 47);
		tputs(BUF_OVERFLOW, 1, pchr);
		if (micli->termcaps.ding)
			tputs(micli->termcaps.ding, 1, pchr);
		if (micli->termcaps.flash)
			tputs(micli->termcaps.flash, 1, pchr);
	}
}

/*
** This function is a wrapper around ft_memdup that sets the proper error flag
** and aborts the program in case of lack of memory. Errno 28 is for lack of
** memory.
*/

void	*clean_ft_memdup(void const *mem, size_t memsize, t_micli *micli)
{
	void	*ptr;

	ptr = ft_memdup(mem, memsize);
	if (!ptr)
	{
		micli->syserror = 28;
		exit_failure(micli);
	}
	return (ptr);
}
