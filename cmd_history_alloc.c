/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_history_alloc.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 14:48:44 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/21 21:50:03 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

void	cmdhist_ptr_array_alloc(t_micli *micli, t_cmdhist *cmdhist)
{
	cmdhist->ptrs_in_hist++;
	if (cmdhist->ptrs_in_hist > cmdhist->cmdhist_buf)
	{
		cmdhist->cmdhist_buf += CMDHIST_BUF;
		//REALLOC
		cmdhist->hist_stack = ft_realloc(cmdhist->hist_stack, \
		(cmdhist->cmdhist_buf + 1) * sizeof(char *), \
		(cmdhist->cmdhist_buf - CMDHIST_BUF) * sizeof(char *), micli);
	}
	cmdhist->hist_stack[cmdhist->ptrs_in_hist] = clean_ft_strdup(micli->buffer, \
	micli);

	//DEBUG CODE
	size_t i = cmdhist->ptrs_in_hist;
	while (i)
	{
		printf("CMDHIST %zu: %s\n", i, cmdhist->hist_stack[i]);
		i--;
	}
}

// void	cmdhist_ptr_array_alloc(t_micli *micli, t_cmdhist *cmdhist)
// {
// 	cmdhist->ptrs_in_hist++;
// 	if (cmdhist->ptrs_in_hist > cmdhist->cmdhist_buf)
// 	{
// 		cmdhist->cmdhist_buf += CMDHIST_BUF;
// 		if (!cmdhist->hist) //ALLOC
// 			cmdhist->hist = clean_calloc(cmdhist->cmdhist_buf + 1, \
// 			sizeof(char *), micli);
// 		else //REALLOC
// 			cmdhist->hist = ft_realloc(cmdhist->hist, \
// 			(cmdhist->cmdhist_buf + 1) * sizeof(char *), (cmdhist->cmdhist_buf - CMDHIST_BUF + 1) * sizeof(char *), micli);
// 	}
// 	cmdhist->hist[cmdhist->ptrs_in_hist - 1] = clean_ft_strdup(micli->buffer, \
// 	micli);

// 	//DEBUG CODE
// 	// size_t i = 0;
// 	// while (i < cmdhist->ptrs_in_hist)
// 	// {
// 	// 	printf("CMDHIST %zu: %s\n", i, cmdhist->hist[i]);
// 	// 	i++;
// 	// }
// }
