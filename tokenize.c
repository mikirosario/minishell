/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 12:20:47 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/08 02:33:05 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Special micli copy function that deletes the character indicated in the
** delete variable from the copy. Source isn't expected to be null terminated
** here, so I don't bother checking for it. We work only with addresses.
**
** The beginning of the string to be copied must be sent as src, and the end as
** src_end. From src to src_end will be copied into dst. Dst is expected to be
** zeroed, so I don't null terminate anything either. Use calloc.
**
** If src is larger than dst your computer explodes.
**
** Not a friendly function. I wouldn't recommend copying it. ;)
*/

char *micli_cpy(char *dst, const char *src, char *src_end, char delete)
{
	char		*d;
	const char	*s;

	d = dst;
	s = src;
	while (s != src_end)
	{
		if (*s != delete && *s != SUB)
			*d++ = *s++;
		else if (*s++ == SUB) //if characters are negative, they are are variable name.for inserting VAR
		{
			*d++ = 'V';
			//copia micli->token->var_lst->content //LA ÚNICA FUNCiÓn que NO RECIBE MICLI AARRRRGHHH
		}
		else
			s++;
	}
	//printf("Src: %s\nDst: %s\n", src, dst);
	return (dst);
}

/*
** This function serves to process a character. If the character is special,
** the appropriate flag is set to indicate it.
**
** The quote flag indicates whether quotes are closed (0), double quotes " are
** open (1) or single quotes ' are open (2). The first check toggles the quote
** flag for the detected chr if either double or single quotes are detected and
** the quote status is closed, if double quotes are detected and the double
** quote status is open, or if a single quote is detected and the single quote
** status is open. This sets the quote status to open if it was closed and a
** quote of either kind is detected, and sets the quote status back to closed if
** it was open and a matching (closing) quote was found.
**
** If the quotes are changing the quote flag (opening or closing quotes) they
** become unprintable, so they are replaced by 127, the ASCII delete flag, to be
** flagged for deletion.
**
** The micli->tokdata.toksize variable keeps track of the token size. For
** memory allocation purposes, opening and closing quotes do not count as they
** will be deleted, so toksize only increments if they are not detected.
**
** An exception to this is if a quote is left open in a token. In bash this is
** handled with multiline commands. Since we don't do multiline commands, we
** assume it is closed and delete it.
**
** If the escape character is used, the escape flag is activated for the next
** character. The escape character is also flagged for deletion with 127.
**
** NOTE: micli->tokdata.toksize always counts one more character than what will
** be saved, because micli->tokdata.tok_end (the index pointer, which is
** pointing to the character we sent as 'chr' in this function) stops on the
** character AFTER the last character to be copied. Thus, we don't need to
** increment micli->tokdata.toksize by 1 for the NULL, as it is already
** included.
**
** This function returns chr, which may be changed as desired.
*/

char			process_char(char *chr, t_micli *micli)
{
	if (!micli->tokdata.escape_flag && *chr == '\\')
	{
		micli->tokdata.escape_flag = 1;
		*chr = DEL; //Flag for deletion
	}
	else if (!micli->tokdata.escape_flag &&
		( (micli->tokdata.quote_flag == 0 && (*chr == '"' || *chr == '\'')) || //if no quotes are open and any quote is found, toggle flag for THAT quote
		(micli->tokdata.quote_flag == 1 && *chr == '"') || //if double quotes are open (state 01 in binary or 1 in decimal) and double quotes are found, toggle double quote flag. this resets to 0.
		(micli->tokdata.quote_flag == 2 && *chr == '\'') ) ) //if single quotes are open and single quotes are found (state 10 in binary or 2 in decimal), toggle single quote flag. this resets to 0.
	{
		micli->tokdata.var_flag = 0; //Bash behaviour, this flag seems to be reset when quotes are opened or closed...
		micli->tokdata.quote_flag = toggle_quote_flag(*chr, micli->tokdata.quote_flag); //check for any quotes and toggle appropriate flag
		*chr = DEL; //Flag for deletion
	}
	else if (!micli->tokdata.escape_flag && micli->tokdata.quote_flag != 2 && *chr == '$' && var_alloc((chr + 1), micli)) //if single quotes are not open and the '$' character is found
	{
		micli->tokdata.var_flag = 1;
		*chr = SUB; //Flag for variable substitution
		micli->tokdata.toksize += get_var_lengths(micli->token->var_lst); //Add all resolved variable string lengths to toksize
	}
	else if (  !micli->tokdata.escape_flag && micli->tokdata.var_flag && (*chr && !ft_isspace(*chr))) //if variable flag is set and no space
	{
		*chr = DEL; //Flag var name for deletion
	}
	else //we do not count micli->tokdata.toksize for opening/closing quotes for memory allocation purposes, as they are deleted.
	{
		micli->tokdata.var_flag = 0;
		micli->tokdata.escape_flag = 0; //reset escape flag
		micli->tokdata.toksize++; //Already counts the NULL, doesn't need to be incremented for null
	}
	return (*chr);
}

/*
** Once the start and end of a token are identified, this function is called to
** reserve enough memory to contain the token and then copy the token to that
** memory location, which will be pointed to from within an instance of the
** token struct.
**
** The token command is pointed to by the token->cmd pointer. A linked list is
** created for command arguments, which will be pointed to by the
** token->arguments pointer.
**
** An argv array is created out of the linked list.
*/

void			process_token(t_micli *micli)
{
	char	*dst;

	ft_printf("Show bytes reserved: %u\n", micli->tokdata.toksize);
	if (!micli->tokdata.cmd_flag) //if micli->tokdata.cmd_flag hasn't been triggered yet, everything from index to &index[i] is the command name
	{
		micli->tokdata.cmd_flag = 1;
		micli->token->cmd = clean_calloc(micli->tokdata.toksize, sizeof(char), micli); //From position 0 at startl to position of index upon flag trigger is the size of the command name
		micli_cpy(micli->token->cmd, micli->tokdata.tok_start, micli->tokdata.tok_end, (char)127); //copy cmd to space pointed to by token->cmd and delete any enclosing quotations. micli_cpy is a special function for this.
	}
	else //if micli->tokdata.cmd_flag has been triggered already, everything from index to &index[i] is an argument
	{
		micli->tokdata.args++; //Increment argument counter, starts at 0. We'll use this to reserve memory for an argument pointer array once the argument list is done.

		//Your typical linked list instructions, for the first list member create the list, for subsequent members create the member and send to the back. The list pointer is at token->arguments.
		//Will clean up the calloc stuff once the rest is cleaned up, I know it's not norm-friendly ;)
		dst = clean_calloc(micli->tokdata.toksize, sizeof(char), micli);
		if (micli->tokdata.args == 1)
			micli->token->arguments = ft_lstnew(dst); //needs to use clean_calloc
		else
			ft_lstadd_back(&micli->token->arguments, ft_lstnew(dst)); //needs to use clean_calloc
		micli_cpy(dst, micli->tokdata.tok_start, micli->tokdata.tok_end, (char)127);
	}
	micli->tokdata.tok_end = ft_skipspaces(micli->tokdata.tok_end); //advance index pointer to beginning of next argument, unless it's endl (which will be a NULL, not a space, so nothing will be skipped)
	micli->tokdata.tok_start = micli->tokdata.tok_end; //token_start pointer points to beginning of next token, or to endl

	// if (micli->token->varnames)
	// 	micli->token->varnames = ft_del(micli->token->varnames);

	//Debug code to ensure copy is correct, remove from final ver
	// ft_printf("Show bytes reserved: %u\n", micli->tokdata.toksize); //Debug code to ensure enough bytes were reserved
	// if (!micli->tokdata.args)
	// 	ft_printf("Command: %s\n", micli->token->cmd);
	// else
	// 	ft_printf("Argument %d: %s\n", micli->tokdata.args, dst);

	micli->tokdata.toksize = 0; //reset string size counter, don't remove this
}

/*
** Once we know where a command begins and ends, we can process it, separating
** the command name from its arguments.
**
** If we want to save the lines of text issued to terminal in a list for later
** retrieval, we can do that here before processing.
**
** Commands should always end in a ';' or null, which will be pointed to by
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
** Any text within quotations will be considered a single argument/command.
**
** Any text after a $ will be considered a variable name (NOTE: NOT YET
** IMPLEMENTED) unless the $ is escaped with '\'.
*/

int		process_command(char *startl, char *endl, t_micli *micli)
{
	t_token			token;
	
	ft_bzero(&token, sizeof(t_token));
	// Save line to array?? Not a requirement, but good feature to have...
	micli->token = &token;
	micli->tokdata.toksize = 0;
	micli->tokdata.args = 0;
	micli->tokdata.cmd_flag = 0; //if this flag is set, everything else we find here is an argument.
	micli->tokdata.tok_start = startl; //start of first token is at line start
	micli->tokdata.tok_end = micli->tokdata.tok_start; //micli->tokdata.tok_end initialized at micli->tokdata.tok_start. It will be incremented until we find the end of the token. When we have a whole token, we process it and move micli->tokdata.tok_start to micli->tokdata.tok_end for next token.
	//Looking for the end of cmds/arguments (aka. tokens)
	while (micli->tokdata.tok_start < endl)
	{
		//if (micli->tokdata.var_flag) //if variable flag is set, this character is the beginning of a variable

		*micli->tokdata.tok_end = process_char(micli->tokdata.tok_end, micli);
		
		//What defines the end of a cmd/argument?
		if ( (micli->tokdata.quote_flag == 0 && (ft_isspace(*micli->tokdata.tok_end))) || micli->tokdata.tok_end == endl ) //if quotes are closed and a space has been found, end of cmd or argument (OR endl has been reached, because we don't do multiline commands)
			process_token(micli);
		else //we handle micli->tokdata.tok_end indexing inside if when we find end of cmd/argument by advancing it to start of next argument.
			micli->tokdata.tok_end++;
	}
	micli->tokdata.quote_flag = 0; //reset quote flag
	micli->tokdata.escape_flag = 0; //reset escape flag
	
	exec_cmd(micli->token->cmd, micli->token->arguments, micli);
	//ft_printf("%c\n", micli->tokdata.quote_flag + 48); Debug code to check quote flag status :)
	free_token(micli); //Free token
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
** until the ; or NULL is passed to the process_command function for
** tokenization.
*/

void	tokenize(char *line, t_micli *micli)
{
	char	*lstart;
	char	*lindex;

	lindex = line; //Start lindex at beginning of line
	while (*lindex) //If we find NULL (could be EOF or \n), always signifies end of command+arguments
	{
		lstart = ft_skipspaces(lindex); //Skip any consecutive spaces to get to start of next command
		lindex = lstart; //set index at start of next command
		while (*lindex && *lindex != ';') //If we find ';' or NULL it signifies end of command+arguments. 
			lindex++;
			//Everything from lstart to lindex is your kingdom, I mean is a whole token (command + arguments). ;) Must be executed before continuing...
		micli->cmd_result = process_command(lstart, lindex, micli); //Pass the address of token start (lstart) and token end (lindex) and process before continuing. 
		//Store command result in cmd_result variable...
		//cmd_result will later be stored in a var named ? so it can be printed with echo $?... when vars are even implemented :p
	}
}