/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal_settings.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 18:05:44 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/30 04:28:49 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Thus function uses ioctl to retrieve the terminal window size and store it
** at micli->winsize. I have basically found one use for this, which is to use
** the col count to calculate how many lines of existing text I need to delete
** to cleanly remove the last prompt when scrolling through the history. I'm
** sure it has more uses than that though. ;)
**
** The define TIOCGWINSZ speaks for itself. TÍO, GET WIN SIZE! YA! The winsize
** struct is defined externally for use with ioctl. Apparently no one can quite
** agree on a single failure mode for ioctl, so it might fail by returning -1,
** or it might fail be reporting the column or row counts as 0. So we control
** for both possibilities. I understand this is a controversal topic. :p
**
** I return 0 for failure and 1 for success, although hopefully it won't fail
** because I haven't actually accounted for get_window_size failing in the
** program... xD
*/

int	get_window_size(t_micli *micli)
{
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &micli->winsize) == -1 \
	 || (micli->winsize.ws_col == 0 || micli->winsize.ws_row == 0))
		return (0);
	return (1);
}

/*
** This function partially enables raw mode. I don't disactivate ICRNL and OPOST
** which translate enter key to return and newline (\r\n) and \n to \r\n,
** respectively. I don't disactive any of the signals either as we use those.
** VMIN and VTIME aren't set either, so read will still wait indefinitely for
** input, as that is desired functionality here.
**
** TCSAFLUSH -- waits for all pending output to be written to terminal, discards
** any input that hasn't yet been read.
**
** TCSADRAIN -- waits for all pending input to be read before changing settings.
** We use it for EOF compatibility if an EOF is sent while the shell is busy.
*/

void	enable_raw_mode(struct termios *raw_term, struct termios *orig_term)
{
	ft_bzero(raw_term, sizeof(struct termios));
	*raw_term = *orig_term;
	raw_term->c_iflag |= (IUTF8);
	raw_term->c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON);
	raw_term->c_cflag |= (CS8);
	raw_term->c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSADRAIN, raw_term);
}
