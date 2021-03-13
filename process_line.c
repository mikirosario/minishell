/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 12:20:47 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/13 21:53:13 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
** Special micli copy function that deletes the character indicated in the
** delete variable from the copy. Source isn't expected to be null terminated
** here, so I don't bother checking for it. We work only with addresses.
**
** The beginning of the string to be copied must be sent as src, and the end as
** src_end. From src to src_end will be copied into dst. Dst is expected to be
** zeroed, so I don't null terminate anything either. Use calloc.
**
** If DEL is found, the character is not copied. If SUB is found, the character
** is substituted for the next resolved variable in var_lst.
**
** If src is larger than dst your computer explodes.
**
** Not a friendly function. I wouldn't recommend copying it. ;)
*/

char 	*micli_cpy(char *dst, const char *src, char *src_end, t_micli *micli)
{
	t_list		*var_lst;
	char		*d;
	const char	*s;
	size_t		i;

	d = dst;
	s = src;
	var_lst = micli->token.var_lst;
	while (s != src_end)
	{
		if (*s != DEL && *s != SUB)
			*d++ = *s++;
		else if (*s == SUB)
		{
			i = 0;
			while (((char *)var_lst->content)[i])
				*d++ = ((char *)var_lst->content)[i++];
			var_lst = var_lst->next;
			s++;
		}
		else
			s++;
	}
	return (dst);
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

int		process_cmdline(char *startl, char *endl, t_micli *micli)
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
			//clear_token(micli);
		}		
		else
			micli->tokdata.tok_end++;
	}
	micli->pipe_flag = toggle_pipe_flag(*micli->tokdata.tok_end, micli->pipe_flag);
	micli->tokdata.quote_flag = 0;
	micli->tokdata.escape_flag = 0;
	exec_cmd(micli->cmdline.cmd, micli->cmdline.arguments, micli);
	clear_cmdline(micli);
	return (0);	
}

/*
** This function parses a single line read from STDIN or a file to delineate
** the beginning and end of a command and all its arguments (undifferentiated).
**
** EOF or \n within the line has been translated to '\0' by this stage, so it is
** null-terminated.
**
** The pointer lstart is pointed to the beginning of the command + arguments.
** It starts by pointing to the beginning of the line. Spaces are skipped.
**
** The pointer lindex is used to iterate through the line from lstart until a ;
** or NULL (meaning \n or EOF) is found, at which point the fragment from lstart
** until the ; or NULL is passed to the process_cmdline function for
** tokenization.
**
** If the last command line ended in ';' we might be at the start of a new
** pipeline. If pipes are then detected in the new command line, we are
** definitely at the start of a new pipeline. If that is the case, we do a
** pipe_reset to prepare the pipeline.
**
** Test functionality with this sequence:
** echo 2*2 | bc | cat; echo this \| "is |" 'a |' test; echo 2+3 | bc 
*/

void	process_raw_line(char *line, t_micli *micli)
{
	char	*lstart;
	char	*lindex;
	unsigned char	quote_flag;
	unsigned char	escape_flag;
	char	cmdline_end;

	cmdline_end = ';';
	escape_flag = 0;
	lindex = line; //Start lindex at beginning of line
	if (!syntax_check(line))
		return ;
	quote_flag = 0;
	while (*lindex) //If we find NULL, always signifies end of line. If we find CMDLINE_END repeated, it's syntax error.
	{
		lstart = lindex; //set start at start of next command
		if (cmdline_end == ';' && (micli->pipes.count = pipe_count(lstart, micli)))
			pipe_reset(&micli->pipes, micli);
		//Everything in this while is part of a cmdline. We leave the while when something that is not part of the command line is found. This may be NULL (stands in for \n and EOF), or special characters ';' or '|'.
		//Quoted or escaped special characters become normal. Characters cannot be escaped between quotes, except '"', '$' and '\' between double quotes only.
		while (*lindex && ((quote_flag || escape_flag) || (*lindex != ';' && *lindex != '|'))) //If we find ';' or '|' or NULL it signifies end of command+arguments cmdline (if not between quotes). If quotes are opened and not closed we leave with NULL (EOL), meaning end of raw line, so we exit the function and quote flag will be reset on reentry into this function.
		{
			escape_flag = 0;
			if (*lindex == '\\' && (quote_flag != 2 && !( quote_flag == 1 && ft_memchr(DQUOTE_ESC_CHARS, *(lindex + 1), 3) == NULL ) ))
			{
				escape_flag = 1;
				lindex++;
			}
			if (!escape_flag)
				quote_flag = toggle_quote_flag(*lindex, quote_flag);
			lindex++;
		}
		//Everything from lstart to lindex is your kingdom, I mean is a whole cmdline (command + arguments). ;) Must be executed before continuing...
		//If the command is piped, all piped commands will be executed in sequence.
		process_cmdline(lstart, lindex, micli); //Pass the address of token start (lstart) and token end (lindex) and process before continuing. 

		//Store command result in cmd_result variable...
		//cmd_result will later be stored in a var named ? so it can be printed with echo $?... when vars are even implemented :p


		//We arrive here with end of last command line, which may be a NULL, a space, a ';' or a '|'. We advance one if we have ';' or '|' and skip any remaining spaces. There should only be one command line end between commands as the syntax_check ensures this before we get here.
		if ((cmdline_end = *lindex)) //If end of last command line was not NULL, advance one.
			lindex++;
		//if ((cmdline_end = *lindex) == ';' || *lindex == '|')
		//	lindex++;
		lindex = ft_skipspaces(lindex); //If after skipping all the spaces after the last command line we find a NULL, it's end of line after all.
	}
}
