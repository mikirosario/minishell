/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 12:06:33 by mrosario          #+#    #+#             */
/*   Updated: 2021/04/01 09:01:10 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Any program failure leading to program termination should immediately save
** errno to the micli->syserror variable, clean up as needed, and then call this
** function.
**
** This function will print the appropriate error message and kill the program.
**
** If ENOTTY sys error was returned, or a -3 init failure (tcgetattr failed) is
** present, it means we could not get the terminal settings, so we have no way
** to reset them - so we don't try to do that as the orig_term struct will be
** empty in that case. Otherwise, we reset the terminal attributes to their
** original state before exiting.
**
** If the program fails due to an internal error, rather than a system error,
** the message "Unknown fatal error" will be displayed.
*/

void	exit_failure(t_micli *micli)
{
	if (micli->syserror != ENOTTY && micli->termcaps.init_result != -3)
		tcsetattr(STDIN_FILENO, TCSANOW, &micli->orig_term);
	sys_error(micli);
	freeme(micli);
	exit(EXIT_FAILURE);
}

/*
** Successful program termination should call this function.
*/

void	exit_success(t_micli *micli)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &micli->orig_term);
	freeme(micli);
	exit(EXIT_SUCCESS);
}
