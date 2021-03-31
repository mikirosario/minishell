/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal_settings.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 18:05:44 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/31 06:24:22 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	termcaps_init_fail(t_micli *micli, int failure, char *termtype)
{
	if (!termtype || !*termtype)
		printf("💥 TERM is unset. Set a terminal type: export TERM=<type>\n");
	else if (failure == -1)
		printf("💥 Could not access the termcap database.\n");
	else if (failure == 0)
		printf("💥 Terminal type '%s' not found in database.\n", termtype);
	else
		return ;
	exit_failure(micli);
}

/*
** First we use find_var to find the TERM environment variable. The find_var
** function is my version of getenv, but unlike getenv it returns a pointer to
** the key, rather than the value, so if the key is found we need to also skip
** it and make sure the value exists after the key. If either of these checks
** fail... that's a paddling.
**
** If a termtype is found at the TERM variable, we try to load its associated
** database into the term buffer with tgetent. If that fails it will return 0 or
** -1.
**
** We send all the results to termcaps_init_fail. If any failure happened along
** the way, it will print the appropriate error message and abort the program.
**
** 
*/

void	termcaps_init(t_micli *micli, t_termcaps *tcaps)
{
	char	*termtype;
	int		tgetent_result;

	termtype = find_var("TERM", 4, micli->envp);
	if (termtype && *(termtype += 5))
		tgetent_result = tgetent(tcaps->termbuf, termtype);	
	termcaps_init_fail(micli, tgetent_result, termtype);
}

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
