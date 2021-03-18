/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   char_operations.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 21:46:33 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/18 21:49:32 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function sets the escape flag and flags the escape character for
** deletion.
*/

void	escape_operations(char *chr, t_micli *micli)
{
	micli->tokdata.escape_flag = 1;
	*chr = DEL;
}

/*
** This function will set the quote flag to reflect the quote type and status
** (see the toggle_quote_flag function for details). The quote character will be
** flagged for deletion.
*/

void	quote_operations(char *chr, t_micli *micli)
{
	micli->tokdata.quote_flag = \
	toggle_quote_flag(*chr, micli->tokdata.quote_flag);
	*chr = DEL;
}

/*
** This function will execute operations when the start of a series of redirect,
** instructions has been found in the command line.
**
** The redir_start pointer is set to point to this character, and the function
** find_redir_end is used find the last character in the subsequent series of
** redirect instructions, the address of which is then saved to the
** redir_end pointer, both living within the cmdline struct. Toksize is not
** counted for these characters, as they are only interpreted, not saved or
** copied.
*/

void	redir_start_operations(char *chr, t_micli *micli)
{
	micli->cmdline.redir_start = chr;
	micli->cmdline.redir_end = find_redir_end(chr);
}

/*
** This function will execute operations when the end of a series of redirect
** instructions has been found in the command line.
**
** In that case we simply reset the redir_start and redir_end pointers to NULL.
**
** Note that after redir_start has been triggered and until redir_end is
** triggered, parsing will continue to happen normally, but the information in
** the corresponding tokens will be sequestered from the argument list and
** interpreted by the redirect routine in the process_token function.
*/

void	redir_end_operations(t_micli *micli)
{
	micli->cmdline.redir_start = NULL;
	micli->cmdline.redir_end = NULL;
}

/*
** This function will execute operations when a the start of a variable is
** detected and the variable flag has been set successfully by var_alloc.
**
** The character will be flagged as SUB to indicate that it be substituted for
** the corresponding resolved variable value. The token size counter will be
** incremented by the size of the resolved variable value.
*/

void	variable_start_operations(char *chr, t_micli *micli)
{
	*chr = SUB;
	micli->tokdata.toksize += get_var_lengths(micli->token.var_lst);
}
