/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   terminal_settings.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 18:05:44 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/20 21:37:14 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function partially enables raw mode. I don't disactivate ICRNL and OPOST
** which translate enter key to return and newline (\r\n) and \n to \r\n,
** respectively. I don't disactive any of the signals either as we use those.
**
** TCSAFLUSH -- waits for all pending output to be written to terminal, discards
** any input that hasn't yet been read.
*/

void	enable_raw_mode(struct termios *raw_term, struct termios *orig_term)
{
	ft_bzero(raw_term, sizeof(struct termios));
	*raw_term = *orig_term;
	raw_term->c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON);
	raw_term->c_cflag |= (CS8);
	raw_term->c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, raw_term);
}
