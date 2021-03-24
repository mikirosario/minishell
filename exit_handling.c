/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 12:06:33 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/20 18:35:20 by mrosario         ###   ########.fr       */
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
** If the program fails due to an internal error, rather than a system error,
** the message "Unknown fatal error" will be displayed.
*/

void	exit_failure(t_micli *micli)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &micli->orig_term);
	sys_error(micli);
	freeme(micli);
	exit(EXIT_FAILURE);
}

/*
** Successful program termination should call this function.
*/

void	exit_success(t_micli *micli)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &micli->orig_term);
	freeme(micli);
	exit(EXIT_SUCCESS);
}
