/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_cmdline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/14 15:32:51 by miki              #+#    #+#             */
/*   Updated: 2021/04/04 12:17:35 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function defines the conditions for considering a character within a
** command line to be the end of a token. If the end of the command line (endl)
** is found, it is always the end of a token, regardless of the flags.
** Otherwise:
**
** The quote flag must be zero, because quoted text always constitutes a single
** token. The character must not be escaped, as the escape flag designifies
** special characters. Additionally, we must find either a space or a redirect
** instruction.
**
** These conditions can be understood as follows.
**
** A token ends IF:
**
** 1. Quote flag is ZERO, and
** 2. Escape flag is ZERO, and
** 3. Space, '>' or '<' or '|' or ';' is found, OR
**
** 1. End of line (endl) is found.
**
** End of line is checked last as it's the least likely to be found.
*/

int	is_token_end(char *endl, t_micli *micli)
{
	int	result;

	result = 0;
	if (((!micli->tokdata.quote_flag && !micli->tokdata.tok_end_escape_flag) && \
	(ft_isspace(*micli->tokdata.tok_end) || *micli->tokdata.tok_end == '>' \
	 || *micli->tokdata.tok_end == '<')) || micli->tokdata.tok_end == endl)
		result = 1;
	micli->tokdata.tok_end_escape_flag = 0;
	return (result);
}

/*
** These conditions define a character that pertains to a single, continuous
** command line. A command line is a line of text with commands, arguments or
** redirect instructions.
**
** The inverse of the conditions for a command line character are the conditions
** for the end of a command line. Thus, when this function returns false, we
** have reached the end of a command line. Unescaped ';' defines the end of a
** command line. An unescaped '|' (pipe) also defines the end of a command line.
** A null character always defines the end of a command line (no multiline in
** minishell).
**
** Thus, the command line conditions are:
**
** 1. The character "exists" (it's not '\0'), AND
** 	a. The quote flag or the escape flag are active, OR
**	b. The character is neither ';' nor '|'.
**
** This is logically equivalent to following inverse conditions, which define
** the end of a command line:
**
** 1. The character is '\0', OR
**
** 1. The quote flag and escape flag are 0, AND
** 2. The character is either ';' or '|'.
*/

int	is_cmdline(char chr, unsigned char escape_flag, \
unsigned char quote_flag)
{
	if (chr && ((quote_flag || escape_flag) || (chr != ';' && chr != '|')))
		return (1);
	return (0);
}

/*
** Once we know where a command begins and ends, we can process it, separating
** the command name from its arguments.
**
** If we want to save the lines of text issued to terminal in a list for later
** retrieval, we can do that here before processing.
**
** Commands should always end in a ';', '|' or null, which will be pointed to by
** endl.
**
** To understand how this function works, it is important to understand the
** quote_flag.
**
** The quote_flag is a two bit unsigned integer in the following format:
**
** single:double
**
** It records the state of quotes in a line as follows:
**
** 00 (0) = All quotes closed.
** 01 (1) = Double quotes opened, single quotes closed.
** 10 (2) = Single quotes opened, double quotes closed.
** 11 (3) = Double quotes opened, single quotes opened.
**
** Note that we do not allow for state 11 (3) to exist. This is because if
** either type of quotation is found in a string, its flag is set (01 or 10, as
** appropriate), and then the only quote condition that may change the flag
** state is for a corresponding closing quotation to be found in the string
** (" or ', respectively).
**
** That is, if " is open, the only way to check for ' is first to close "", and
** if ' is open, the only way to check for " is first to close ''.
**
** Currently, if quotes are left open, the opened quotes will be passed as an
** argument if separated from the last argument by a space, or as part of the
** current argument if not separated by a space. That is:
**
** '"command"'"		->	arg1: "command""
**
** '"command"' "	->	arg1: "command" arg2: "
**
** This isn't bash behaviour, as bash will wait for you to close commas in
** terminal, but the subject specifically says not to implement multiline
** commands, so this is the micli resolution for open quotes.
**
** This function first checks for an unescaped '='. If found, everything before
** it is treated as a var name and everything after it is treated as the name's
** assigned value. (NOTE: NOT YET IMPLEMENTED)
**
** Otherwise, it will parse the string to separate the command from its
** arguments and execute the command.
**
** Any text within quotations will be considered a single argument/command, aka
** 'token'.
**
** Any text after a $ will be considered a variable name unless the $ is escaped
** with '\', between single quotes ' ', or isn't a valid varchar (see isvarchr
** function for more details).
**
** If the end of a command line is a '|' then the pipe flag needs to be set. The
** toggle_pipe_flag function handles this.
*/

int	process_cmdline(char *startl, char *endl, t_micli *micli)
{
	ft_bzero(&micli->token, sizeof(t_token));
	ft_bzero(&micli->tokdata, sizeof(t_tokendata));
	micli->tokdata.tok_start = startl;
	micli->tokdata.tok_end = micli->tokdata.tok_start;
	while (micli->tokdata.tok_start < endl)
	{
		*micli->tokdata.tok_end = process_char(micli->tokdata.tok_end, micli);
		if (is_token_end(endl, micli))
		{
			process_token(micli);
			micli->token.var_lst = ft_lstfree(micli->token.var_lst);
		}
		else
			micli->tokdata.tok_end++;
	}
	micli->pipe_flag = \
	toggle_pipe_flag(*micli->tokdata.tok_end, micli->pipe_flag);
	micli->tokdata.quote_flag = 0;
	micli->tokdata.escape_flag = 0;
	micli->cmd_result = 0;
	exec_cmd(micli->cmdline.cmd, micli->cmdline.arguments, micli);
	clear_cmdline(micli);
	return (0);
}
