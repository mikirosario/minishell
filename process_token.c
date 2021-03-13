/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_token.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/13 21:20:16 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/13 21:34:41 by mrosario         ###   ########.fr       */
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
** 3. Space, '>' or '<' is found, OR
**
** 1. End of line (endl) is found.
**
** End of line is checked last as it's the least likely to be found.
*/

int		is_token_end(char *endl, t_micli *micli)
{
	if ((!micli->tokdata.quote_flag && !micli->tokdata.escape_flag && \
	(ft_isspace(*micli->tokdata.tok_end) || *micli->tokdata.tok_end == '>' || \
	*micli->tokdata.tok_end == '<')) || micli->tokdata.tok_end == endl)
		return (1);
	return (0);
}

/*
** Once the start and end of a token are identified, this function is called to
** reserve enough memory to contain the token and then copy the token to that
** memory location.
**
** The token containing the command is pointed to by the token->cmd pointer. A
** linked list is created for tokens containing arguments to the command, which
** is pointed to by the cmdline->arguments pointer.
**
** An argv array will later be created out of token->cmd and the linked list of
** arguments.
**
** If the redir_end flag has been set, it is because redirect instructions are
** now being read. If that is the case, all tokens until the redir_end flag is
** unset will be sequestered in a separate linked list from the command
** arguments, which will be pointed to by the cmdline->redir_tokens pointer.
**
** These tokens will contain the file names to be opened. Any environment
** variables used within those names should resolve normally by micli_cpy.
**
** If the redir_end flag has been set, then the token will be treated as part of
** series of redirect instructions and sequestered. It will be interpreted as
** according to whether it is ">" ">>" or "<" or not. If it is ">", ">>" or "<"
** a function will be called to set the redir_out or redir_in flag as
** appropriate. Otherwise it will be interpreted as a path and opened/created
** as a file.
**
** NOTE: Redirect instructions ">" or "<" can be found either as token ends or
** as lone tokens. This is because, for example, "echo 2>out" will return ">" as
** the end of the token beginning at "e". If all spaces are then skipped, ">"
** will then be the start of the token beginning at ">" and the end of the same
** token. Likewise, "echo 2 > out" will give ">" as a token beginning at ">" and
** ending at ">". Thus, when tok_start == tok_end, we know we are looking at a
** redirect instruction. (Keep in mind that the characters '<' and '>' are
** ignored for token delimitation purposes if escaped in any way).
**
** The index pointer micli->tokdata->tok_end will then skip any spaces and
** advance to the beginning of the next token. If a ';' or '\0' is found then
** that will be the same address as endl and the next token will be a new
** command line, so the program should leave the calling function,
** process_cmdline, and re-enter it.
**
** If the redir_end flag has been set, then after spaces are skipped, any '<' or
** '>' will be skipped, and spaces will be skipped again to get to the next file
** name. Might alter this to save read modes too.
**
** If a '|' is found then that will be the same address as endl and the next
** token will be the start of a new command line, and the output of the current
** command line must be piped to it. This will set the pipe flag.
**
** Otherwise the next token will be an argument to the current command line, or
** a file name for the current redirect.
**
** Use this at the start of the function to check bytes reserved for each token:
** ft_printf("Show bytes reserved: %u\n", micli->tokdata.toksize);
*/

void	process_token(t_micli *micli)
{
	if (!micli->tokdata.cmd_flag)
	{
		micli->tokdata.cmd_flag = 1;
		micli->cmdline.cmd = clean_calloc(micli->tokdata.toksize, sizeof(char), micli);
		micli_cpy(micli->cmdline.cmd, micli->tokdata.tok_start, micli->tokdata.tok_end, micli);
	}
	else if (micli->cmdline.redir_end && ((micli->tokdata.tok_end > micli->cmdline.redir_start && micli->tokdata.tok_end <= micli->cmdline.redir_end) || micli->tokdata.tok_start == micli->tokdata.tok_end))
	{
		if (micli->tokdata.tok_start == micli->tokdata.tok_end)
			interpret_redir_instruction(micli->tokdata.tok_end, micli);
		else
			open_redir_file(micli);
	}
	else if (micli->tokdata.tok_start != micli->tokdata.tok_end)
		get_argument(micli);
	micli->tokdata.tok_end = ft_skipspaces(micli->tokdata.tok_end);
	if ((*micli->tokdata.tok_end == '>' || *micli->tokdata.tok_end == '<') && micli->tokdata.tok_start == micli->tokdata.tok_end)
	{
		while (*micli->tokdata.tok_end == '>' || *micli->tokdata.tok_end == '<')
			micli->tokdata.tok_end++;
		micli->tokdata.tok_end = ft_skipspaces(micli->tokdata.tok_end);
	}
	micli->tokdata.tok_start = micli->tokdata.tok_end;
	micli->tokdata.toksize = 0;
}