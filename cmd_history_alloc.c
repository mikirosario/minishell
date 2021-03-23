/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_history_alloc.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 14:48:44 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/23 22:30:49 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


/*
** This is one funky function. This function manages the history stack for
** micli. The history stack is a null-terminated character pointer array.
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
** Here is a schema of what is going on, where 0 is the scratch array, \0 is
** the NULL and \n is some \n-terminated string:
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
** The function ft_free_split can be (and is) used to free this array.
*/

void	push_to_hist_stack(t_micli *micli, short *active_line, \
t_cmdhist *cmdhist)
{
	//char *scratch_buf;

	cmdhist->ptrs_in_hist++;
	//scratch_buf = cmdhist->hist_stack[cmdhist->ptrs_in_hist - 2];
	//scratch_buf_cpy = clean_ft_strdup(scratch_buf, micli);
	if (cmdhist->ptrs_in_hist > cmdhist->cmdhist_buf) //1 scratch pointer, rest hist pointers
	{
		cmdhist->cmdhist_buf += CMDHIST_BUF;
		//REALLOC
		cmdhist->hist_stack = ft_realloc(cmdhist->hist_stack, \
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
			strlen = ft_strlen16(cmdhist->hist_stack[i]);
		else
			strlen = 0;
		size_t j = 0;
		while (j < strlen)
		{
			write(1, &cmdhist->hist_stack[i][j], 2);
			j++;
		}

		// write(STDOUT_FILENO, "CMDHIST ", 7);
		// printf("%zu: ", i);
	 	// write(STDOUT_FILENO, cmdhist->hist_stack[i], strlen * sizeof(short));
	 	printf("\n");
	}
}

/*
** This function adds every line from terminal (ended in '\n') to an array of
** strings called cmdhist->hist. The array is buffered with CMDHIST_BUF. Every
** new line saved in the history increments the counter cmdhist->ptrs_in_hist by
** one.
**
** If adding a new line would cause an overflow the buffer is reallocated with
** CMDHIST_BUF more pointers as necessary.
**
** Once the buffer is confirmed as prepared to accept the new line, a copy is 
** made of micli->buffer (where) raw lines from terminal are stored and the last
** free member of cmdhist->hist is made to point to it. The position of the last
** free member of cmdhist->hist is ptrs_in_hist - 1 as ptrs_in_hist is a size,
** not a length.
*/

// void	cmdhist_ptr_array_alloc(t_micli *micli, t_cmdhist *cmdhist)
// {
// 	cmdhist->ptrs_in_hist++;
// 	if (cmdhist->ptrs_in_hist > cmdhist->cmdhist_buf)
// 	{
// 		cmdhist->cmdhist_buf += CMDHIST_BUF;
// 		//REALLOC
// 		cmdhist->hist_stack = ft_realloc(cmdhist->hist_stack,
// 		(cmdhist->cmdhist_buf + 1) * sizeof(char *),
// 		(cmdhist->cmdhist_buf - CMDHIST_BUF) * sizeof(char *), micli);
// 	}
// 	cmdhist->hist_stack[cmdhist->ptrs_in_hist] = clean_ft_strdup(micli->buffer,
// 	micli);

// 	//DEBUG CODE
// 	size_t i = cmdhist->ptrs_in_hist;
// 	while (i)
// 	{
// 		printf("CMDHIST %zu: %s\n", i, cmdhist->hist_stack[i]);
// 		i--;
// 	}
// }

// void	cmdhist_ptr_array_alloc(t_micli *micli, t_cmdhist *cmdhist)
// {
// 	cmdhist->ptrs_in_hist++;
// 	if (cmdhist->ptrs_in_hist > cmdhist->cmdhist_buf)
// 	{
// 		cmdhist->cmdhist_buf += CMDHIST_BUF;
// 		if (!cmdhist->hist) //ALLOC
// 			cmdhist->hist = clean_calloc(cmdhist->cmdhist_buf + 1,
// 			sizeof(char *), micli);
// 		else //REALLOC
// 			cmdhist->hist = ft_realloc(cmdhist->hist,
// 			(cmdhist->cmdhist_buf + 1) * sizeof(char *), (cmdhist->cmdhist_buf - CMDHIST_BUF + 1) * sizeof(char *), micli);
// 	}
// 	cmdhist->hist[cmdhist->ptrs_in_hist - 1] = clean_ft_strdup(micli->buffer,
// 	micli);

// 	//DEBUG CODE
// 	// size_t i = 0;
// 	// while (i < cmdhist->ptrs_in_hist)
// 	// {
// 	// 	printf("CMDHIST %zu: %s\n", i, cmdhist->hist[i]);
// 	// 	i++;
// 	// }
// }
