/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 18:17:50 by mrosario          #+#    #+#             */
/*   Updated: 2021/01/31 18:26:07 by mrosario         ###   ########.fr       */
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
		if (*s != delete)
			*d++ = *s++;
		else
			s++;
	}
	return (dst);
}




/*
** Function to toggle a bit between 0 and 1. Returns bit-set variable.
**
** 0 is the rightmost bit.
**
** If bit is set higher than 8, behaviour is undefined.
*/

unsigned char	ft_togglebit(unsigned char byte, unsigned char bit)
{
	unsigned char	mask;

	mask = (unsigned char)1 << bit;
	return (mask ^ byte);
}

/*
** Frees all memory reserved for a character pointer array with ft_split, first
** freeing the lines pointed to by each pointer, then freeing the pointer array
** itself.
*/

char	**free_split(char **split)
{
	int	i;

	i = 0;
	while (split[i])
	{
		if (split)
		split[i] = ft_del(split[i]);
		i++;
	}
	free(split);
	return (NULL);
}

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
	if (micli->syserror)
		ft_printf("\n%s\n", strerror(micli->syserror)); //make ft_realloc set errno, or use internal error handling :p
	else
		ft_printf("\nUnknown fatal error\n");
	exit(EXIT_FAILURE);
}

/*
**
*/

 /*char	*str_tok(char *line, t_micli *micli)
 {
	char	**tokens;

	tokens = ft_split(line, ' ');
	micli->syserror = errno;
	if (!tokens)
	exit_failure(micli);

	int	i; //temporal
	i = 0;
	while (tokens[i])
		ft_printf("%s ", tokens[i++]);
	write(1, "\n", 1);
	tokens = free_split(tokens);
	return(NULL);
 }*/

/*
** Finds first 'c' in line and return its address.
**
** If passed a null pointer, returns null.
**
** If line_end is passed and is greater than line, find will search only until
** the specified line_end. Otherwise, it will search until it finds a NULL char.
*/

char	*find(char *line, char *line_end, char c)
{
	if (line)
	{
		if (line_end && line_end > line)
			while (line < line_end && *line != c)
				line++;
		else
			while (*line && *line != c)
				line++;
	}
	return (line);
}

/*
** Update the quotes flag if quotes are detected.
**
** The quote_flag is a two bit unsigned integer in the following format:
**
** single:double
**
** 00 (0) = All quotes closed.
** 01 (1) = Double quotes opened, single quotes closed.
** 10 (2) = Single quotes opened, double quotes closed.
** 11 (3) = Double quotes opened, single quotes opened.
*/

unsigned char	check_quotes(char quotes, char quote_flag)
{
	if (quotes == '"')
		quote_flag = ft_togglebit(quote_flag, 0);
	else if (quotes == '\'')
		quote_flag = ft_togglebit(quote_flag, 1);
	return(quote_flag);
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
	t_token	token;
	char	*tok_start;
	char	*tok_end;
	char	*arg_start;
	unsigned int		args;
	char	cmd_flag;
	size_t	strsize;

	char *tmp_arg_ptr;
	
	// Save line to array?? Not a requirement, but good feature to have...

	strsize = 0;
	args = 0;
	cmd_flag = 0; //if this flag is set, everything else we find here is an argument.
	tok_start = startl; //start of first token is at line start
	tok_end = tok_start; //tok_end initialized at tok_start. It will be incremented until we find the end of the token. When we have a whole token, we process it and move tok_start to tok_end for next token.
	//Looking for the end of cmds/arguments (aka. tokens)
	while (tok_start < endl)
	{
		if ((micli->token.quote_flag == 0 && (*tok_end == '"' || *tok_end == '\'')) || //if no quotes are open and any quote is found, toggle flag for THAT quote
			(micli->token.quote_flag == 1 && *tok_end == '"') || //if double quotes are open (state 01 in binary or 1 in decimal) and double quotes are found, toggle double quote flag. this resets to 0.
			(micli->token.quote_flag == 2 && *tok_end == '\'')) //if single quotes are open and single quotes are found (state 10 in binary or 2 in decimal), toggle single quote flag. this resets to 0.
		{
			if (micli->token.quote_flag != 0)
				micli->token.quote = *tok_end; //Copy the quotation type to a variable so we can delete it from the copy (for closed quotes only).
			micli->token.quote_flag = check_quotes(*tok_end, micli->token.quote_flag); //check for any quotes and toggle appropriate flag
		}
		else //we do not count strsize for opening/closing quotes for memory allocation purposes, as they are deleted.
			strsize++; //NOTE: strsize always counts one more character than what will be saved, because tok_end stops on the character after the last character to be copied. Thus, we don't  need to increment strsize by 1 for the NULL. However, when we leave open quotes, the open quotes are printed... so we can leave 1 extra byte for that, just in case... :p
		
		//What defines the end of a cmd/argument?
		if ( (micli->token.quote_flag == 0 && (ft_isspace(*tok_end))) || tok_end == endl ) //if quotes are closed and a space has been found, end of cmd or argument (OR endl has been reached, because we don't do multiline commands)
		{
			if (!cmd_flag) //if cmd_flag hasn't been triggered yet, everything from index to &index[i] is the command name
			{
				cmd_flag = 1;
				token.cmd = ft_calloc(strsize + 1, sizeof(char)); //From position 0 at startl to position of index on flag trigger is the size of the command name, plus 1 for null termination
				micli_cpy(token.cmd, tok_start, tok_end, micli->token.quote); //copy cmd and delete any enclosing quotations.
				tok_end = ft_skipspaces(tok_end); //advance index pointer to beginning of next argument, unless it's endl
				tok_start = tok_end;
				arg_start = tok_start; //arguments start at the token following the first token
				ft_printf("Bytes reserved: %u\n", strsize + 1);
				strsize = 0; //reset string size counter
				ft_printf("%s\n", token.cmd);
			}
			else //if cmd_flag has been triggered already, everything from index to &index[i] is an argument
			{
				args++;

				if (args == 1)
					token.arguments = ft_lstnew( tmp_arg_ptr = ft_calloc(strsize + 1, sizeof(char)) );
				else
					ft_lstadd_back(&token.arguments, ( ft_lstnew( tmp_arg_ptr = ft_calloc(strsize + 1, sizeof(char)) ) ) );
				micli_cpy(tmp_arg_ptr, tok_start, tok_end, micli->token.quote);
				tok_end = ft_skipspaces(tok_end); //advance index pointer to beginning of next argument, unless it's endl
				tok_start = tok_end;
				ft_printf("Bytes reserved: %u\n", strsize + 1);
				strsize = 0; //reset string size counter
				ft_printf("Argument %d: %s\n", args, tmp_arg_ptr);
				/*if (tok_end != endl)
				{
					tok_start = ft_skipspaces(tok_end); //advance index pointer to beginning of next argument, unless it's endl
					tok_end = tok_start;
				}
				else
				{
					token.arguments = malloc(sizeof(char *) * args);

				}*/
				//ft_printf("Here be %d arguments arrgh\n", args);		
			}
		}
		else //we handle tok_end indexing inside if when we find end of cmd/argument by advancing it to start of next argument.
			tok_end++;
	}
	micli->token.quote_flag = 0; //reset quote flag
	//ft_printf("%c\n", micli->token.quote_flag + 48); Debug code to check quote flag status :)
	return (0);	
}

/*
** EOF or \n within line has been translated to \0 by this stage.
*/

 char	*str_tok(char *line, t_micli *micli)
 {
	char	*lstart;
	char	*lindex;

	(void)lstart;
	lindex = line; //Start lindex at beginning of line
	while (*lindex) //If we find NULL (could be EOF or \n), always signifies end of command
	{
		lstart = ft_skipspaces(lindex); //Skip any consecutive spaces to get to start of next command
		lindex = lstart; //set index at start of next command
		while (*lindex && *lindex != ';') //If we find ';' or NULL it signifies end of command. 
			lindex++;
			//Everything from lstart to lindex is your kingdom, I mean is a whole token (command + arguments). ;) Must be executed before continuing...
		micli->cmd_result = process_command(lstart, lindex, micli); //Pass the address of token start (lstart) and token end (lindex) and process before continuing. 
		//Store command result in cmd_result variable...
		//cmd_result will later be stored in a var named ? so it can be printed with echo $?... when vars are even implemented :p
	}
	return (lindex);
 }



/*
** This function reallocates the memory of the string pointed to by ptr to a
** new memory block of the size defined by size, freeing the old memory block.
** 
** If a null pointer is passed, a null pointer will be returned and nothing
** will be freed. Freeing a null pointer results in no operation being
** performed, so it's fine.
**
** If the reallocation fails, the old memory block is destroyed and a null
** pointer is returned. Errno at failure is saved to a variable in the micli
** struct for use in subsequent error handling.
**
** This function uses memcpy, which is not safe if dst and src overlap.
**
** Behaviour is undefined if size is less than the memory to be reallocated.
** Probably means segfault. So just don't do that. ;)
*/

char	*ft_realloc(char *ptr, size_t size, t_micli *micli)
{
	char *tmp;

	tmp = ptr;
	if (!ptr || !(ptr = malloc(sizeof(char) * size)))
		micli->syserror = errno;
	else
		ft_memcpy(ptr, tmp, size);
	//printf("Bufsize: %zu\n", size); (Debug code)
	tmp = ft_del(tmp);
	return (ptr);
}

/*
** This function defines a buffer for reading from STDIN in steps determined by
** the READLINE_BUFSIZE constant.
**
** It will try to reserve memory for READLINE_BUFSIZE bytes. If it fails it
** will print the message pointed to by the result of strerror(errno) (should
** be no. 28 "No space left on device") and initiate program termination.
**
** When reading from STDIN the read() function hangs until '\n' newline or
** terminal EOF is input, after which it will read the first thing in its
** buffer in a chunk defined by the third argument and copy it to the address
** passed as the second argument. For every subsequent call it will read the
** subsequent chunk in the buffer until the newline or EOF. If the third
** argument defines a chunk size that is larger than what remains
** in the buffer to be read, the smaller size will be read. After reaching
** newline or EOF it will hang again pending another newline or EOF. Newline
** counts as a character.
**
** Note that read will NOT clear the high bytes of the memory location passed
** as the second argument if the chunk to be copied is smaller than specified
** in the third argument, so the value copied will not be as expected unless
** the high bytes are zeroed.
*/

char	*micli_readline(t_micli *micli)
{
	size_t	size;

	size = 0;
	micli->bufsize = READLINE_BUFSIZE;
	micli->position = 0;
	if (!(micli->buffer = malloc(sizeof(char) * micli->bufsize)))
	{
		micli->syserror = errno;
		micli->buffer = ft_del(micli->buffer);
		exit_failure(micli); //gestionar con flags
	}
	while (1)
	{
		size += read(STDIN_FILENO, &micli->c, 1); //Si no ha leído nada se comporta como salto de línea?? O_O Lee último carácter, salto de línea? Se queda colgado en read???
		//If we read EOF or newline was input by user, null terminate buffer, print and return it.
		if (micli->c == EOF || micli->c == '\n')
		{
			micli->buffer[micli->position] = '\0';
			//ft_printf("%s\n", micli->buffer); //esto imprime último comando a stdout
			return (micli->buffer);
		}
		//Otherwise, add the character to our buffer and advance one byte.
		else
			micli->buffer[micli->position] = micli->c;
		micli->position++;
		//If after advancing we exceed the allocated buffer size, expand buffer by READLINE_BUFSIZE bytes and reallocate.
		//Podemos usar funciones de libft para esto, para de momento lo hago un poco chapuceramente. ;p
		if (micli->position >= micli->bufsize)
		{
			micli->bufsize += READLINE_BUFSIZE;
			if (!(micli->buffer = ft_realloc(micli->buffer, micli->bufsize, micli)))
				exit_failure(micli);
		}
	}
	
	return ("return");	
}

char	micli_loop(t_micli *micli)
{
	char shutdown;
	char *line;
	
	shutdown = 0;

	while (!shutdown)//no parece que esté usando shutdown...
	{
		//write(1, "TEST", 4);
		write(1, "🚀 ", 6);
		line = micli_readline(micli);//stays here...
		//write(1, "TEST", 4);
		if (!(ft_strcmp(line, "exit")))
		{
				line = ft_del(line);
				exit(EXIT_SUCCESS); //gestionar con flags
		}
		else
		{
			str_tok(line, micli);
		}
		/*
		//ls "implementation" ;)
		if (!(ft_strcmp(line, "ls")))
		{
			DIR *d;
			struct dirent *dir;
			d = opendir(".");
			if (d)
			{
				while ((dir = readdir(d)) != NULL)
					ft_printf("%s\n", dir->d_name);
				closedir(d);
			}
		}
		if (!(ft_strcmp(line, "pwd")))
		{
			char cwd[FILENAME_MAX];
			if (getcwd(cwd, sizeof(cwd)) != NULL)
				ft_printf("%s\n", cwd);
		}
		if (!(ft_strcmp(line, "cd ..")))
		{
			chdir("..");
		}
		// waiting for token
		// if (!(ft_strcmp(line, "cd")))
		// {
		// 	opendir("..");
		// }*/
		line = ft_del(line);
	}
	return (0);
}

int 	main(int argc, char **argv)
{
	t_micli micli;

	ft_bzero(&micli, sizeof(t_micli));
	//config files
	(void)argc;
	(void)argv;

	//command loop
	micli_loop(&micli);

	//shutdown and cleanup
	return (0);
}
