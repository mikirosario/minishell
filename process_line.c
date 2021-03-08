/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_line.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 12:20:47 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/08 23:31:03 by miki             ###   ########.fr       */
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
		}
		else
			s++;
	}
	//printf("Src: %s\nDst: %s\n", src, dst);
	return (dst);
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
** The '<' or '>' or ">>" redirects are not saved, so additional function is
** needed to set the read/write mode appropriately... will see how >I do this.
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
	//HERE WE NEED REDIRECT FLAG TO SEQUESTER REDIRECT DATA...
	//If the redirect_end pointer is set and we are either past the start of the redirect instructions and
	// before the end of them(before the end check probably superflous) OR token start == token end,
	// which means we hit a '>' as hitting a '>' also now define token end and further into this function I trap
	// the pointer on '>' until start == end condition is met.
	else if (micli->cmdline.redir_end && ((micli->tokdata.tok_end > micli->cmdline.redir_start && micli->tokdata.tok_end <= micli->cmdline.redir_end) || micli->tokdata.tok_start == micli->tokdata.tok_end))
	{
		//USE THIS TO TURN REDIRECT TOKEN LIST BACK ON FOR DEBUGGING; ALSO REACTIVATE REDIR_TOKENS IN HEADER
		// dst = clean_calloc(micli->tokdata.toksize, sizeof(char), micli);
		// if (!micli->cmdline.redir_tokens)
		// 	micli->cmdline.redir_tokens = ft_lstnew(dst); //needs to use clean_calloc
		// else
		// 	ft_lstadd_back(&micli->cmdline.redir_tokens, ft_lstnew(dst)); //needs to use clean_calloc
		if (micli->tokdata.tok_start == micli->tokdata.tok_end) //if tok_start == tok_end there is a '>' or '<' or ">>", so we want the associated instruction data. Is it a write append, write trunc or read instruction?
		{
			if (*micli->tokdata.tok_end == '>' && *(micli->tokdata.tok_end + 1) == '>') //for '>>' copy 2 chars '>>'
			{
				//USE THIS TO TURN REDIRECT TOKEN LIST BACK ON FOR DEBUGGING
				//micli_cpy(dst, micli->tokdata.tok_start, (micli->tokdata.tok_end + 2), micli);
				micli->cmdline.redir_out_flag = 2; //write append mode
			}
			else
			{
				//USE THIS TO TURN REDIRECT TOKEN LIST BACK ON FOR DEBUGGING
				//micli_cpy(dst, micli->tokdata.tok_start, (micli->tokdata.tok_end + 1), micli); //for '<' or '>' copy 1 char
				if (*micli->tokdata.tok_end == '>')
					micli->cmdline.redir_out_flag = 1; //write trunc mode
				else
					micli->cmdline.redir_in_flag = 1; //read mode
			}
			// if (*dst == '>')
			// 	micli->cmdline.redir_out = ft_lstlast(micli->cmdline.redir_tokens); //if output instruction, save address to its list member to redir_out
			// else
			// 	micli->cmdline.redir_in = ft_lstlast(micli->cmdline.redir_tokens); //if input instruction, save a pointer to its list member to redir_in
		}
		else //If it's not a redirect instruction, it's a file name, so do this
		{
			dst = clean_calloc(micli->tokdata.toksize, sizeof(char), micli); //reserve memory for a copy of the file name
			micli_cpy(dst, micli->tokdata.tok_start, micli->tokdata.tok_end, micli); //copy the file name; each file name is its own token, so tok_start to tok_end is the file name
			if (micli->cmdline.redir_out_flag) //now, depending on the preceding instruction flag we've set...
			{
				if (micli->cmdline.fd_redir_out) //If a file has already been opened, close it
					close(micli->cmdline.fd_redir_out);
				if (micli->cmdline.redir_out_flag == 1) //1 is for trunc
					micli->cmdline.fd_redir_out = open(dst, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); //OPEN IN WRONLY TRUNC MODE, permissions 644
				else //2 is for append
					micli->cmdline.fd_redir_out = open(dst, O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); //OPEN IN WRONLY APPEND MODE, permissions 644
			}
			else if (micli->cmdline.redir_in_flag) //if it's a read instruction
			{
				if (micli->cmdline.redir_in)//if a file has already been opened, close it
					close(micli->cmdline.fd_redir_in);
				micli->cmdline.fd_redir_in = open(dst, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); //OPEN IN RONLY MODE, permissions 644
			}
			dst = ft_del(dst); //free the memory
			micli->cmdline.redir_out_flag = 0; //MOVE THESE TO TOKDATA SO THEY LIVE AS LONG AS TOKEN ONLY...
			micli->cmdline.redir_in_flag = 0; //WHEN TOKDATA IS NOT AN UTTER MESS THAT IS... xD
		}
	}
	//Empieza el guarreo aquí. Resulta que con tanto el espacio no escapado como el '<' o '>' no escapado como condiciones de
	//terminar un token, si entramos aquí con echo test>out, tok_start sería 't' y tok_end sería '>'. Pero en cambio si
	//hacemos echo test > out, el espacio después de test ya provoca que test se recoja como token, y ahora tok_start y
	//tok_end ambos apuntan a '>', con lo que al recogerse se recoge como argumento de un solo byte, que apunta a un
	// carácter nulo. (teniendo en cuenta que tok_end no es inclusive). Para evitar esto, necesitamos esta instrucción de
	// no recoger argumento si no hay nada que recoger (start == end), que en este caso solo se debería dar con los dichosos
	// '<'.
	else if (micli->tokdata.tok_start != micli->tokdata.tok_end) //if micli->tokdata.cmd_flag has been triggered already, everything from index to &index[i] is an argument
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
	if ((*micli->tokdata.tok_end == '>' || *micli->tokdata.tok_end == '<') && micli->tokdata.tok_start == micli->tokdata.tok_end)
	{
		while (*micli->tokdata.tok_end == '>' || *micli->tokdata.tok_end == '<')
			micli->tokdata.tok_end++;
		micli->tokdata.tok_end = ft_skipspaces(micli->tokdata.tok_end);
	}

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
	ft_bzero(&micli->cmdline, sizeof(t_cmdline)); //repeat instruction in clear_cmdline... make up my mind where to put this
	ft_bzero(&micli->tokdata, sizeof(t_tokendata));
	// Save line to array?? Not a requirement, but good feature to have...
	micli->tokdata.tok_start = startl; //start of first token is at line start
	micli->tokdata.tok_end = micli->tokdata.tok_start; //micli->tokdata.tok_end initialized at micli->tokdata.tok_start. It will be incremented until we find the end of the token. When we have a whole token, we process it and move micli->tokdata.tok_start to micli->tokdata.tok_end for next token.
	//Looking for the end of cmds/arguments (aka. tokens)
	while (micli->tokdata.tok_start < endl)
	{
		*micli->tokdata.tok_end = process_char(micli->tokdata.tok_end, micli);
		
		//What defines the end of a token?
		// '<' or '>' will now define start of special redir token... redir token must be interpreted but supressed from arguments
		if ( ( !micli->tokdata.quote_flag && !micli->tokdata.escape_flag && (ft_isspace(*micli->tokdata.tok_end) || *micli->tokdata.tok_end == '>' || *micli->tokdata.tok_end == '<')) || micli->tokdata.tok_end == endl ) //if quotes are closed and a space has been found or a redirect order has been found, end of token (OR if endl has been reached, because we don't do multiline commands)
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
	// t_list *tmp = micli->cmdline.redir_tokens;
	// while (tmp)
	// {
	// 	ft_printf("Sequestered Redirect Instruction: %s\n", tmp->content);
	// 	tmp = tmp->next;
	// }
	// tmp = micli->cmdline.arguments;
	// int tmpctr = 1;
	// while (tmp)
	// {
	// 	ft_printf("Argument %d: %s\n", tmpctr++, tmp->content);
	// 	tmp = tmp->next;
	// }
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
