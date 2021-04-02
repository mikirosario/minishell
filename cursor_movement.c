/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cursor_movement.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/02 05:24:00 by miki              #+#    #+#             */
/*   Updated: 2021/04/02 06:22:04 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// void	get_affected_lines(t_micli *micli, unsigned short char_total)
// {
// 	int	occupied_lines;

// 	get_window_size(micli);
// }


void	wrap_down_left(t_micli *micli, t_termcaps *tcaps)
{
	(void)micli;
	tputs(tcaps->carriage_ret, 1, pchr);
	tputs(tcaps->scroll_up, 1, pchr);
	tputs(tcaps->arrow_down, 1, pchr);
}


void	move_cursor_left(t_micli *micli)
{
	char cursor_pos;

	if (micli->termcaps.curpos_buf > 2)
	{
		cursor_pos = check_horizontal_cursor_pos();
		if (cursor_pos == 1)
			wrap_up_right(micli, &micli->termcaps);
		else
			tputs(micli->termcaps.cur_left, 1, pchr);
		micli->termcaps.curpos_buf--;
	}
}

void	move_cursor_right(t_micli *micli)
{
	char cursor_pos;

	if (micli->termcaps.curpos_buf < *micli->cmdhist.char_total + 2)
	{
		cursor_pos = check_horizontal_cursor_pos();
		get_window_size(micli);
		if (cursor_pos == micli->winsize.ws_col)
			wrap_down_left(micli, &micli->termcaps);
		else
			tputs(micli->termcaps.cur_right, 1, pchr);
		micli->termcaps.curpos_buf++;
	}
}