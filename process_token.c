/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_token.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/13 21:20:16 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/19 03:07:29 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** A token will be considered a redirect instruction if:
**
** 1. The redir_end flag has been set, pointing to the end of the series of
** redirect instructions to which it belongs, AND,
** 	a. The address of the index used by the process_token function,
** micli->tokdata.tok_end, is less than or equal to the address pointed to by
** the redir_end flag OR,
**	b. The address of tok_start is equal to the address of tok_end.
**
** Note that to be considered a redirect instruction, the tok_end address must
** be GREATER THAN, NOT greater than or equal to, the redir_start address. This
** is because process_token processes the text PRECEDING tok_end. So, for:
**
** tok_start tok_end
**		↓	 ↓
** echo test > file;
**			 ↑	   ↑
** redir_start	   redir_end
**
** The token that will be processed is "test", an ARGUMENT, NOT "file", a
** redirect instruction! So when tok_end == redir_start, it is NOT a redirect
** instruction, but rather an argument... usually. xD There is an exception.
**
** Special condition 1.b. is necessary because (unescaped) redirect operators
** '>' or '<' or '>>' are defined as ending tokens. Thus, when the token
** preceding a redirect operator is read, the next token will be the redirect
** operator itself. For the first operator in a redirect instruction set, this
** will mean tok_end == redir_start again, but this time tok_start == tok_end
** ALSO:
**
**		  tok_start
**		  tok_end
**			 ↓
** echo test > file;
**			 ↑	   ↑
** redir_start	   redir_end
**
** Hence, case 1.b. is if redir_end has been set AND tok_start == tok_end, then
** this is also a redirect instruction. Bit convoluted, but it works. Yes, I did
** redirects last. Can you tell? xD
*/

int		is_redir_instruction(t_micli *micli)
{
	if (micli->cmdline.redir_end && \
	((micli->tokdata.tok_end > micli->cmdline.redir_start && \
	micli->tokdata.tok_end <= micli->cmdline.redir_end) || \
	micli->tokdata.tok_start == micli->tokdata.tok_end))
		return (1);
	return (0);
}

/*
** If the command flag for a cmdline has not yet been set, this function is
** called. It sets the command flag to indicate that a command has been found.
** It reserves enough memory to contain the current token and makes a copy of
** the current token using the special micli_cpy function to an address pointed
** to by micli->cmdline.cmd.
*/

void	get_command(t_micli *micli)
{
	micli->tokdata.cmd_flag = 1;
	micli->cmdline.cmd = clean_calloc(micli->tokdata.toksize, \
	sizeof(char), micli);
	micli_cpy(micli->cmdline.cmd, micli->tokdata.tok_start, \
	micli->tokdata.tok_end, micli);
}

/*
** This function copies a token as an argument and places it in an argument
** list. The argument list is stored in the cmdline struct and remains valid for
** the life of a command line.
**
** The argument counter is incremented to count the arguments in a command line.
** It is later used to reserve memory for the argv array.
**
** NOTE: This function reserves memory that must be freed later by the function
** that frees all memory occupied by the argument list. This function is called
** ft_lstclear and is called by clear_cmdline.
**
** NOTE: The ft_lstnew function reserves memory but won't exit gracefully if it
** fails like clean_calloc does... sorry. :p
*/

void	get_argument(t_micli *micli)
{
	char *dst;

	micli->tokdata.args++;
	dst = clean_calloc(micli->tokdata.toksize, sizeof(char), micli);
	if (micli->tokdata.args == 1)
		micli->cmdline.arguments = ft_lstnew(dst);
	else
		ft_lstadd_back(&micli->cmdline.arguments, ft_lstnew(dst));
	micli_cpy(dst, micli->tokdata.tok_start, micli->tokdata.tok_end, micli);
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
** If the redir_end flag has been set, then the token will be treated as part of
** series of redirect instructions and sequestered. It will be interpreted as
** according to whether it is ">" ">>" or "<" or not. If it is ">", ">>" or "<"
** a function will be called to set the redir_out or redir_in flag as
** appropriate. Otherwise it will be interpreted as a path and opened/created
** as a file.
**
** NOTE: Redirect operators ">" or "<" can be found either as token ends or as
** lone tokens. This is because, for example, "echo 2>out" will return ">" as
** the end of the token beginning at "e". If all spaces are then skipped, ">"
** will then be the start of the token beginning at ">" and the end of the same
** token. Likewise, "echo 2 > out" will give ">" as a token beginning at ">" and
** ending at ">". Thus, when tok_start == tok_end, we know we are looking at a
** redirect instruction. (Keep in mind that the characters '<' and '>' are
** ignored for token delimitation purposes if escaped in any way). This will NOT
** be the case for other single-character arguments as, since they aren't
** end-token conditions like unescaped redirect operators are, their tok_end
** address will ALWAYS be the following character (even if the following
** character is a null character).
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
** NOTE: The logic that when tok_start == tok_end we have a redirect operator is
** used here too to decide when it is necessary to skip ahead.
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
		get_command(micli);
	else if (is_redir_instruction(micli))
	{
		if (micli->tokdata.tok_start == micli->tokdata.tok_end)
			interpret_redir_operator(micli->tokdata.tok_end, micli);
		else
			open_redir_file(&micli->tonti, micli);
	}
	else if (micli->tokdata.tok_start != micli->tokdata.tok_end)
		get_argument(micli);
	micli->tokdata.tok_end = ft_skipspaces(micli->tokdata.tok_end);
	if ((*micli->tokdata.tok_end == '>' || *micli->tokdata.tok_end == '<') \
	&& micli->tokdata.tok_start == micli->tokdata.tok_end)
	{
		while (*micli->tokdata.tok_end == '>' || *micli->tokdata.tok_end == '<')
			micli->tokdata.tok_end++;
		micli->tokdata.tok_end = ft_skipspaces(micli->tokdata.tok_end);
	}
	micli->tokdata.tok_start = micli->tokdata.tok_end;
	micli->tokdata.toksize = 0;
}
