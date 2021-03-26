/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_history_alloc.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 14:48:44 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/26 08:18:54 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


/*
** This is one funky function. This function manages the history stack for
** micli. The history stack is a null-terminated short pointer array.
**
** The lowest memory address in the array corresponds to the oldest entry. The
** penultimate address in the array is used as a scratch log for editing a new
** terminal line, and is dynamically allocated and reallocated in the
** micli->readline function. All addresses before the scratch log address are
** historical entries, which can at any time be made active and re-edited.
**
** The variable cmdhist->ptrs_in_hist, when used as a position in the array,
** points to the terminating NULL. Thus, cmdhist->ptrs_in_hist - 1 always points
** to the scratch log.
**
** The active_line is a duplicate of the \n-terminated line that was sent out
** from micli_readline. This may be the scratch log or it may be a history
** entry. It will now be added to the history, so ptrs_in_hist is incremented.
**
** The old scratch log (cmdhist_stack[ptrs_in_hist - 2]) is freed and replaced
** with a copy of the active_line as the newest history entry. The following
** pointer in the array will be used by micli->readline to address the new
** scratch log.
**
** If insufficient pointers are available in the buffer to accept a new entry,
** the array buffer will be expanded and the pointer array will be reallocated.
** We reallocate with calloc, so we don't bother to copy the NULL termination.
** NOTE: It is only the pointers that are being reallocated to a larger array
** here, and the pointer addresses that are being copied over to the new array,
** NOT the memory addressed by the pointers, which is reallocated as needed by
** micli->readline.
**
** Here is a schema of what is going on, where 0 is the scratch log, \0 is the
** NULL and \n is some \n-terminated string of shorts:
**
** active_line
**	    v  v
**  0  \n \n -> 0 == oldest
**	\0►►0 \n -> 1 == newest
**	   \0►►0 -> 2 == scratch
**		  \0 -> 3 == NULL
**
** We start with scratch and NULL. When the first line is sent, the previous
** scratch log becomes the newest entry, the previous NULL termination becomes
** the new scratch log, and the NULL termination after that becomes the new
** NULL terminator. When the second line is sent the active line (which may be
** the scratch log or the history entry) becomes the newest entry, the preceding
** entry remains as an older entry, the previous NULL termination becomes the
** new scratch log, and the NULL termination after *that* becomes the new NULL
** terminator. Etc, as much as your memory will allow. ;)
**
** The function ft_free_short_split is used to free this array.
**
** A null check at the beginning of this function prevents empty lines from
** being saved to the command history, as in bash. New line characters are
** converted to null characters before being onforwarded by micli_readline.
** Lines thus point to a single null character when enter is pressed with no
** text in the buffer. This causes the push_to_hist_stack function to
** return immediately with no operation.
*/

void	push_to_hist_stack(t_micli *micli, short *active_line, \
t_cmdhist *cmdhist)
{
	if (!*active_line)
		return ;
	cmdhist->ptrs_in_hist++;
	if (cmdhist->ptrs_in_hist > cmdhist->cmdhist_buf)
	{
		cmdhist->cmdhist_buf += CMDHIST_BUF;
		cmdhist->hist_stack = clean_realloc(cmdhist->hist_stack, \
		(cmdhist->cmdhist_buf + 1) * sizeof(short *), \
		(cmdhist->ptrs_in_hist - 1) * sizeof(short *), micli);
	}
	cmdhist->hist_stack[cmdhist->ptrs_in_hist - 2] = ft_del(cmdhist->hist_stack[cmdhist->ptrs_in_hist - 2]);
	cmdhist->hist_stack[cmdhist->ptrs_in_hist - 2] = clean_ft_memdup(active_line, cmdhist->active_line_size * sizeof(short), micli);
	//DEBUG CODE
	size_t i = cmdhist->ptrs_in_hist + 1;
	size_t strlen;
	while (i--)
	{
		printf("CMDHIST %zu: ", i); fflush(stdout);
		if (cmdhist->hist_stack[i])
			strlen = ft_strlen16(&cmdhist->hist_stack[i][2]);
		else
			strlen = 0;
		size_t j = 2;
		while (j < strlen + 2)
		{
			write(1, &cmdhist->hist_stack[i][j], 2);
			j++;
		}

		// write(STDOUT_FILENO, "CMDHIST ", 7);
		// printf("%zu: ", i);
	 	// write(STDOUT_FILENO, cmdhist->hist_stack[i], strlen * sizeof(short));
	 	printf("\n");
	}
	//DEBUG CODE
}
