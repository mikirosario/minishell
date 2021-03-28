/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_char.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/26 04:20:38 by miki              #+#    #+#             */
/*   Updated: 2021/03/28 00:42:48 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function serves to process a character in a token. If the character is
** special, the appropriate flag is set to indicate it so special things can be
** done with it. ;) This function prepares characters in a token to be copied
** to a clean string for use in micli_argv, or for use by the redirect
** sequester, flagging characters for deletion or substitution as needed.
**
** If the escape character is used, the escape flag is activated for the next
** character. The escape character is replaced with 127, the ASCII delete flag,
** to be flagged for deletion by micli_cpy. Escaped characters are never
** special, but not all characters can be escaped all the time.
**
** The quote flag indicates whether quotes are closed (0), double quotes " are
** open (1) or single quotes ' are open (2). Opening and closing quotes are
** also flagged for deletion with 127.
**
** The micli->tokdata.toksize variable keeps track of the token size. For
** memory allocation purposes, any characters flagged for deletion do not count
** towards the token size, while any characters flagged for substitution will
** increment the token size by the size of the data for which they will be
** substituted.
**
** If quotes are left open when a line ends, in bash this is handled with
** multiline commands. Since we don't do multiline commands in minishell, in
** micli we simply assume they are closed at the end of the line, so you should
** get the same result as if you immediately closed the quotes in the next bash
** line.
**
** NOTE: micli->tokdata.toksize always counts one more character than what will
** be copied, because micli->tokdata.tok_end (the index pointer used in the
** calling function, which is pointing to the character we sent as 'chr' in this
** function) stops on the character AFTER the last character to be copied. Thus,
** we don't need to increment micli->tokdata.toksize by 1 for the NULL, as it is
** already included.
**
** This function returns chr, which may be modified by it as desired.
**
** Characters in a command line will be sent to this function for analysis in
** sequential order of appearance. This function has special instructions and
** general instructions. The general instructions are contained in the last else
** in the sequence and apply when none of the special instructions apply. There
** is also a variable check at the beginning that will reset the variable flag
** if the end of a variable name is found.
**
** This function will analyse a character as follows and execute the
** corresponding instructions:
**
** NOTE: An active escape flag will PREVENT ANY special instructions from being
** carried out!!
**
** ////VARIABLE END CHECK\\\\
** - Is the character the end of a variable name?
**	The variable flag will be set to NULL.
**
** ////REDIRECT START/END CHECK (MUTUALLY EXCLUSIVE)\\\\
** - Is the character the start of a series of redirect instructions?
**	The redir_start pointer is set to point to this character, and the function
**	find_redir_end is used find the last character in the series of redirect
**	instructions, which is saved to the redir_end pointer, both living within
**	the cmdline struct. Toksize is not counted for these characters, as they are
**	only interpreted, not saved or copied.
**
** - Is the character the end of a series of redirect instructions?
**	If the current character address is greater than the redir_end address, then
**	we have passed the last redirect_instruction set, so we reset the
**	redir_start and redir_end pointers to NULL. This will only be done if
**	redir_end is set to begin with, otherwise neither start nor end instructions
**	will be executed.
**
** ////SPECIAL INSTRUCTIONS (MUTUALLY EXCLUSIVE)\\\\
** - Is the character an escape character?
**	The escape flag will be set and the character flagged for deletion.
**
** - Is the character opening or closing quotes?
**	The quote flag will be set to reflect the quote status (see the
**	toggle_quote_flag function for details). The character will be flagged for
**	deletion.
**
** - Is the character the start of a new variable?
**	The variable flag will be set to indicate that subsequent characters are
**	part of the variable name. The character will be flagged as SUB to indicate
**	that it be substituted for the corresponding variable value. The token size
**	counter will be incremented by the size of the resolved variable value.
**	NOTE: A set variable flag is a pointer to the last character in the variable
**	name and will be unset by the variable check at the beginning of the
**	function when that character is reached.
**
** - Is the character part of a variable name?
**	If the variable flag is set, it indicates that this character belongs to a
**	variable name. The character will thus be flagged for deletion, as it is
**	useless to us once the variable has been resolved (which happens in the
**	preceding else if).
**
** If none of the special instructions are applicable, then the general
** instructions for a generic character will be as follows:
**
** ////GENERAL INSTRUCTIONS (MUTUALLY EXCLUSIVE WITH SPECIAL INSTRUCTIONS)\\\\
** 1. Reset the escape flag (an active escape flag will prevent ANY special
** instructions from being executed, so general instructions will always be the
** ones applied to an escaped character).
** 2. Increment the token size counter to count the character.
**
** Note that for whatever reason, bash escapes even unescapable characters when
** they are *not* between double quotes. For example:
** bash: "\lol" -> \lol
** bash: \lol -> lol
*/

char	process_char(char *chr, t_micli *micli)
{
	if (is_variable_end(chr, micli->tokdata.var_flag))
		micli->tokdata.var_flag = NULL;
	if (is_redirect_start(*chr, micli->tokdata.escape_flag, \
	micli->tokdata.quote_flag, micli->cmdline.redir_end))
		redir_start_operations(chr, micli);
	else if (micli->cmdline.redir_end && micli->cmdline.redir_end < chr)
		redir_end_operations(micli);
	if (*chr && is_escape_char(*chr, *(chr + 1), micli->tokdata.escape_flag, \
	micli->tokdata.quote_flag))
		escape_operations(chr, micli);
	else if (is_quote_char(*chr, micli->tokdata.escape_flag, \
	micli->tokdata.quote_flag))
		quote_operations(chr, micli);
	else if (is_variable_start(chr, &micli->tokdata, micli))
		variable_start_operations(chr, micli);
	else if (micli->tokdata.var_flag)
		*chr = DEL;
	else
	{
		micli->tokdata.escape_flag = 0;
		micli->tokdata.toksize++;
	}
	return (*chr);
}
