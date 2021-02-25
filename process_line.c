/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 12:20:47 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/25 21:55:46 by mrosario         ###   ########.fr       */
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

char *micli_cpy(char *dst, const char *src, char *src_end, t_micli *micli)
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
		else if (*s == SUB) //if SUB is detected, it is replaced with the resolved variables stored in var_lst for this token in order of appearance.
		{
			i = 0;
			while (((char *)var_lst->content)[i])
				*d++ = ((char *)var_lst->content)[i++];
			var_lst = var_lst->next;
			s++;
		// // 	//copia micli->token.var_lst->content //LA ÚNICA FUNCiÓn que NO RECIBE MICLI AARRRRGHHH
		}
		else
			s++;
	}
	//printf("Src: %s\nDst: %s\n", src, dst);
	return (dst);
}

/*
** This function tests a character in a string to see whether it meets the
** criteria to be considered an escape character by minishell.
**
** The character will NOT be considered an escape character if:
**
** 1. It is NOT a backslash ('\').
** 2. It is itself escaped.
** 3. It is between single quotes (where no characters are escapable).
** 4. It is between double quotes AND the next character is not escapable (double
** quotes admit only three escapable characters, which are '"', '$' and '\').
**
** This is semantically the same as saying that the character WILL be considered
** an escape character if:
**
** 1. The character is a backslash ('\').
** 2. The character is NOT itself escaped.
** 3. The character was NOT found between single quotes.
** 4. If it is NOT TRUE that the character was found between double quotes AND
** the next character is not one of the three escapable characters between
** double quotes, which are '"', '$' and '\'.
**
** The latter was the original formulation, however I inverted the logic and
** switched to the former becuase the latter requires all conditions to be
** checked to come to a decision, whereas the former only requires one of the
** checks to fail to come to a decision. This means the former logic can be
** ordered from most likely to least likely failure conditions in order to save
** CPU cycles, which is what I have attempted to do. The double quote check is
** actually a composite of three checks, so it goes last.
*/

int				is_escape_char(char chr, char next_chr, unsigned char escape_flag, unsigned char quote_flag)
{
	if (chr != '\\' || escape_flag || quote_flag == 2 || (quote_flag == 1 && ft_memchr(DQUOTE_ESC_CHARS, next_chr, 3) == NULL))
		return (0);
	return (1);
}

/*
** This function tests a character in a string to see whether it meets the
** criteria to be considered an opening or closing quotes character by
** minishell.
**
** The character WILL be considered an opening or closing quotes character if:
**
** 1. The character is unescaped AND:
**	a.	The character is not between quotes AND the character is either a double
**		or a single quote (" or '), OR
**	b. 	The character is between double quotes AND the character is a double
**		quote.
** OR
** 2. The character is between single quotes AND the character is a single
** quote.
**
** This formulation checks the escape flag first, as an active escape flag will
** falsify the check for two quote flag statuses (no quotes or open double
** quotes), then the quote flag as for any quote status except no quotes, only
** a single character check is necessary rather than two.
*/

int				is_quote_char(char chr, unsigned char escape_flag, unsigned char quote_flag)
{
	if ( (!escape_flag && //if unescaped
		 ((quote_flag == 0 && (chr == '"' || chr == '\'')) || //if no quotes are open and any quote is found, toggle flag for THAT quote
		(quote_flag == 1 && chr == '"') ) ) || //if double quotes are open (state 01 in binary or 1 in decimal) and double quotes are found, toggle double quote flag. this resets to 0.
		(quote_flag == 2 && chr == '\'') ) //regardless whether or not escaped (though they never should be), if single quotes are open and single quotes are found (state 10 in binary or 2 in decimal), toggle single quote flag. this resets to 0.
		return (1);
	return (0);
}

/*
** This function tests a character in a string to see whether it meets the
** criteria to be considered a variable. This also depends on the result of the
** var_alloc function, which determines whether the variable can be located and
** tries to save its location to memory. When found, variable locations are
** saved on a per-token basis in a linked list called var_list in order of their
** appearance in the token.
**
** The character WILL be considered a variable if:
**
** 1. It is unescaped.
** 2. It is not between single quotes.
** 3. It is a '$'.
** 4. The corresponding value location is successfully saved to memory by
** var_alloc.
**
** This is semantically the same as saying that the character will NOT be
** considered a variable if:
**
** 1. It is NOT '$', OR
** 2. It is escaped, OR
** 3. It is between single quotes, OR
** 4. The var_alloc function fails to find a value for it (note, this is not the
** same as failing to find the variable, as non-existent variables will point to
** a null character).
**
** The former was the original formulation. The latter, with inverted logic, was
** implemented because it has exclusive conditions, ordered from most to least
** likely to be true, which should save CPU cycles.
*/

int				is_variable(char chr, char *var_name, t_tokendata *tokdata, t_micli *micli)
{
	//if (!tokdata->escape_flag && tokdata->quote_flag != 2 && chr == '$' && var_alloc(var_name, micli)) //if unescacped, single quotes are not open and the '$' character is found
	if (chr != '$' || tokdata->escape_flag || tokdata->quote_flag == 2 || !var_alloc(var_name, micli))
		return (0);
	return (1);
}

/*
** This function tests a character in a string to see whether it meets the
** criteria to be considered as part of a variable name. Variable names start
** on the character following any unescaped '$' (except between single quotes),
** which is indicated by setting the var_flag. Every character of a variable
** name must be flagged for deletion. When the end of a variable name is
** detected, the var_flag must be reset.
**
** The character WILL be considered part of a variable name if:
**
** 1. It is unescaped. (???)
** 2. It is flagged as such by var_flag.
** 3. It is not a null character AND not a ';' AND not a '|' AND not a space
** (all of which signal the end of a variable name, as does opening or closing quotes...)
)
*/

// int				is_var_name(char chr, unsigned char var_flag, unsigned char escape_flag)
// {
// 	(  !micli->tokdata.escape_flag && micli->tokdata.var_flag && (*chr && *chr != ';' && *chr != '|' && !ft_isspace(*chr))) //if escape flag is not set and variable flag is set and space, '|', NUL or ; is NOT found
// 	return(1);
// }


/*
** This function tests a character in a string to see whether it meets the
** criteria to be considered the end of a variable name. Variable names end when
** the variable flag is active and any character is found that cannot be a valid
** variable name.
**
** The character will be considered the end of a variable name if:
**
** 1. The variable flag is set.
** 2. The isvarchar function returns 0.
*/

int				is_end_var_name(char chr, unsigned char var_flag)
{
	if (var_flag && !isvarchar(chr))
		return (1);
	return (0);
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
**
//////////////////////////////////////////////////////////////////
** Should functionalize conditionals so they are easier to read...
//////////////////////////////////////////////////////////////////
**
** Characters in a command line will be sent to this function for analysis in
** sequential order of appearance. This function has special instructions and
** general instructions. The general instructions are contained in the last else
** in the sequence and apply when none of the special instructions apply.
**
** This function will analyse a charactr as follows and execute the corresponding
** instructions if any of the special conditions prove true:
**
** NOTE: An active escape flag will PREVENT ANY special instructions from being
** carried out!!
**
** ////SPECIAL INSTRUCTIONS\\\\
** - Is the character an escape flag?
**	The escape flag will be set and the character flagged for deletion.
**
** - Is the character opening or closing quotes?
**	The quote flag will be set to reflect the quote status (see the
**	toggle_quote_flag function for details). The character will be flagged for
**	deletion. The variable flag will be reset, so if any variable name
**	comes before an unescaped quote, the variable name will be considered to end
**	there.
**
** - Is the character substituting for a variable?
**	The variable flag will be set to indicate that subsequent characters are
**	part of the variable name. (NOTE: MAY HAVE TO USE ISVARCHAR to further tweak this)
**	The character will be flagged as SUB to indicate that it be substituted for
**	the corresponding variable value. The token size counter will be incremented
**	by the size of the variable value.
**
** - Is the character part of a variable name?
**	The character will be flagged for deletion.
**
** If none of the special instructions are applicable, then the general
** instructions for a generic character will be as follows:
**
** ////GENERAL INSTRUCTIONS\\\\
** 1. Reset the variable flag (if the character is not part of a variable name,
** the third condition among the special instructions, this flag must be reset).
**
** 2. Reset the escape flag (an active escape flag will prevent ANY special
** instructions from being executed, so general instructions will always be the
** ones applied to an escaped character).
**
** 3. Increment the token size counter to
** count the character.
*/

char			process_char(char *chr, t_micli *micli)
{
	//micli->tokdata.escape_flag = 0;
	//Escape flag activates when '\' is detected AND the '\' is not itself escaped AND the '\' is both not between single quotes and not between double quotes but in front of an unescapable character (character not in the dquote_esc_chars list).
	// Note that for whatever reason, bash behaviour escapes even unescapable characters when they are *not* between double quotes. I.e.:
	// bash: "\lol" -> \lol
	// bash: \lol -> lol
	// Don't blame me, I'm just replicating it. :p Since we don't have multiline commands, hanging escapes are just treated as if new-lined.
	//if (*chr == '\\' && !micli->tokdata.escape_flag && (micli->tokdata.quote_flag != 2 && !( micli->tokdata.quote_flag == 1 && ft_memchr(DQUOTE_ESC_CHARS, *(chr + 1), 3) == NULL ) ))
	if (is_end_var_name(*chr, micli->tokdata.var_flag))
		micli->tokdata.var_flag = 0;
	if (is_escape_char(*chr, *(chr + 1), micli->tokdata.escape_flag, micli->tokdata.quote_flag))
	//if (!micli->tokdata.escape_flag && micli->tokdata.quote_flag != 2 && *chr == '\\') //if single quotes are not open and escape flag is found
	{
		micli->tokdata.escape_flag = 1;
		*chr = DEL; //Flag for deletion
	}
	//When are quotes opened or closed?
	//When the quotes are unescaped AND no quotes have been activated and either kind of quotes are found OR double quotes are open and double quotes are found
	//OR, in any case, when single quotes are open and single quotes are found.
	// else if ( (!micli->tokdata.escape_flag && //if unescaped
	// 	 ((micli->tokdata.quote_flag == 0 && (*chr == '"' || *chr == '\'')) || //if no quotes are open and any quote is found, toggle flag for THAT quote
	// 	(micli->tokdata.quote_flag == 1 && *chr == '"') ) ) || //if double quotes are open (state 01 in binary or 1 in decimal) and double quotes are found, toggle double quote flag. this resets to 0.
	// 	(micli->tokdata.quote_flag == 2 && *chr == '\'') ) //regardless whether or not escaped, if single quotes are open and single quotes are found (state 10 in binary or 2 in decimal), toggle single quote flag. this resets to 0.
	else if (is_quote_char(*chr, micli->tokdata.escape_flag, micli->tokdata.quote_flag))
	{
		//micli->tokdata.var_flag = 0; //Bash behaviour, this flag seems to be reset when quotes are opened or closed...
		micli->tokdata.quote_flag = toggle_quote_flag(*chr, micli->tokdata.quote_flag); //check for any quotes and toggle appropriate flag
		*chr = DEL; //Flag for deletion
	}
	//else if (!micli->tokdata.escape_flag && micli->tokdata.quote_flag != 2 && *chr == '$' && var_alloc((chr + 1), micli)) //if unescacped, single quotes are not open and the '$' character is found
	else if (is_variable(*chr, (chr + 1), &micli->tokdata, micli))
	{
		micli->tokdata.var_flag = 1;
		*chr = SUB; //Flag for variable substitution
		micli->tokdata.toksize += get_var_lengths(micli->token.var_lst); //In place of the substituted character, add the RESOLVED variable string length to toksize
	}
	//else if (  !micli->tokdata.escape_flag && micli->tokdata.var_flag && (*chr && *chr != ';' && *chr != '|' && !ft_isspace(*chr))) //if escape flag is not set and variable flag is set and space, '|', NUL or ; is NOT found
	else if (micli->tokdata.var_flag)
	{
		*chr = DEL; //Flag var name for deletion
	}
	else //we do not count micli->tokdata.toksize for any deleted characters for memory allocation purposes (unescaped quotes, variable names, etc). Variables will add the resolved length of their values to toksize.
	{
		//micli->tokdata.var_flag = 0;
		micli->tokdata.escape_flag = 0; //reset escape flag
		micli->tokdata.toksize++; //Already counts the NULL, so doesn't need to be incremented for null
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
**
** The index pointer micli->tokdata->tok_end will then skip any spaces and
** advance to the beginning of the next token. If a ';' or '\0' is found then
** that will be the same address as endl and the next token will be a new
** command line, so the program should leave the calling function,
** process_cmdline, and re-enter it.
**
** If a '|' is found then that will be the same address as endl and the next
** token will be a new command line, and the output of the current command line
** must be piped to it. This will set the pipe flag.
**
** Otherwise the next token will be an argument to the current command line.
*/

void			process_token(t_micli *micli)
{
	char	*dst;

	ft_printf("Show bytes reserved: %u\n", micli->tokdata.toksize);
	if (!micli->tokdata.cmd_flag) //if micli->tokdata.cmd_flag hasn't been triggered yet, everything from index to &index[i] is the command name
	{
		micli->tokdata.cmd_flag = 1;
		micli->cmdline.cmd = clean_calloc(micli->tokdata.toksize, sizeof(char), micli); //From position 0 at startl to position of index upon flag trigger is the size of the command name
		micli_cpy(micli->cmdline.cmd, micli->tokdata.tok_start, micli->tokdata.tok_end, micli); //copy cmd to space pointed to by token->cmd and delete any enclosing quotations. micli_cpy is a special function for this.
	}
	else //if micli->tokdata.cmd_flag has been triggered already, everything from index to &index[i] is an argument
	{
		micli->tokdata.args++; //Increment argument counter, starts at 0. We'll use this to reserve memory for an argument pointer array once the argument list is done.

		//Your typical linked list instructions, for the first list member create the list, for subsequent members create the member and send to the back. The list pointer is at token->arguments.
		//Will clean up the calloc stuff once the rest is cleaned up, I know it's not norm-friendly ;)
		dst = clean_calloc(micli->tokdata.toksize, sizeof(char), micli);
		if (micli->tokdata.args == 1)
			micli->cmdline.arguments = ft_lstnew(dst); //needs to use clean_calloc
		else
			ft_lstadd_back(&micli->cmdline.arguments, ft_lstnew(dst)); //needs to use clean_calloc
		micli_cpy(dst, micli->tokdata.tok_start, micli->tokdata.tok_end, micli);
	}
	//micli->tokdata.tok_end = ft_skipspaces(micli->tokdata.tok_end);
	micli->tokdata.tok_end = ft_skipspaces(micli->tokdata.tok_end);

	//advance index pointer to BEGINNING of next token, unless it's already endl (which will be a NULL, not a space, so nothing will be skipped). Beginning of next token may be its first character or '|' or ';', which ends a command line and means the next token will also be a new command.
	micli->tokdata.tok_start = micli->tokdata.tok_end; //token_start pointer points to beginning of next token, or to endl

	// if (micli->token.varnames)
	// 	micli->token.varnames = ft_del(micli->token.varnames);

	//Debug code to ensure copy is correct, remove from final ver
	//ft_printf("Bytes reserved: %u\n", micli->tokdata.toksize); //Debug code to ensure enough bytes were reserved
	// if (!micli->tokdata.args)
	// 	ft_printf("Command: %s\n", micli->cmdline.cmd);
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
*/

int		process_cmdline(char *startl, char *endl, t_micli *micli)
{
	ft_bzero(&micli->token, sizeof(t_token));
	ft_bzero(&micli->cmdline, sizeof(t_cmdline));
	ft_bzero(&micli->tokdata, sizeof(t_tokendata));
	// Save line to array?? Not a requirement, but good feature to have...
	micli->tokdata.tok_start = startl; //start of first token is at line start
	micli->tokdata.tok_end = micli->tokdata.tok_start; //micli->tokdata.tok_end initialized at micli->tokdata.tok_start. It will be incremented until we find the end of the token. When we have a whole token, we process it and move micli->tokdata.tok_start to micli->tokdata.tok_end for next token.
	//Looking for the end of cmds/arguments (aka. tokens)
	while (micli->tokdata.tok_start < endl)
	{
		//if (micli->tokdata.var_flag) //if variable flag is set, this character is the beginning of a variable

		*micli->tokdata.tok_end = process_char(micli->tokdata.tok_end, micli);
		
		//What defines the end of a token?
		if ( (micli->tokdata.quote_flag == 0 && (ft_isspace(*micli->tokdata.tok_end))) || micli->tokdata.tok_end == endl ) //if quotes are closed and a space has been found, end of token (OR endl has been reached, because we don't do multiline commands)
		{
			process_token(micli);
			micli->token.var_lst = ft_lstfree(micli->token.var_lst); //Free token's variable list, if created
			//clear_token(micli);
		}
		else //we handle micli->tokdata.tok_end indexing inside the preceding if when we find end of a cmd/argument by advancing it to start of next argument.
			micli->tokdata.tok_end++;
	}
	 //If end of cmdline is a '|' we set the pipe flag to indicate that current command must send its output to the next command via the established pipe at micli->pipe.
	micli->pipe_flag = toggle_pipe_flag(*micli->tokdata.tok_end, micli->pipe_flag); //set pipe flag
	micli->tokdata.quote_flag = 0; //reset quote flag
	micli->tokdata.escape_flag = 0; //reset escape flag
	// t_list *tmp = micli->cmdline.arguments;
	// while (tmp)
	// {
	// 	ft_printf("%s\n", tmp->content);
	// 	tmp = tmp->next;
	// }
	exec_cmd(micli->cmdline.cmd, micli->cmdline.arguments, micli);
	//ft_printf("%c\n", micli->tokdata.quote_flag + 48); Debug code to check quote flag status :)
	clear_cmdline(micli); //Free memory reserved for cmdline parsing
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
	//ft_printf("How Many Pipes In This Line?\n%s\n%u\n", line, pipe_count(line, micli));
	

	
	quote_flag = 0;
	while (*lindex) //If we find NULL (could be EOF or \n), always signifies end of command+arguments. If we find CMDLINE_END repeated, syntax error. If we find CMDLINE_END at the beginning of a line, syntax error. If we find CMDLINE_END and/or spaces and after that NULL, end of command+arguments.
	{
		lstart = lindex; //set start at start of next command
		// lstart = ft_skipspaces(lindex); //Skip any consecutive spaces to get to start of next command
		if (cmdline_end == ';' && (micli->pipes.count = pipe_count(lstart, micli))) //If the last command line ended in ';' we might be at the start of a new pipeline; if pipes are then detected in the new command line, we are definitely at the start of a new pipeline...
			pipe_reset(&micli->pipes, micli); //Pipe reset could return error codes to print out failure messages, or something... currently closes shell on failure
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
