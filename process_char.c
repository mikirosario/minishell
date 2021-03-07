/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_char.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/26 04:20:38 by miki              #+#    #+#             */
/*   Updated: 2021/03/07 21:44:11 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


/*
** This function tests a character in a string to see whether it meets the
** criteria to be considered an escape character by minishell.
**
** The character WILL be considered an escape character if:
**
** 1. The character is a backslash ('\'), AND
** 2. The character is NOT itself escaped, AND
** 3. The character was NOT found between single quotes, AND
** 4. If it is NOT TRUE that the character was found between double quotes AND
** the next character is not one of the three escapable characters between
** double quotes, which are '"', '$' and '\'.
**
** This is semantically the same as the inverse logic, stating that the
** character will NOT be considered an escape character if:
**
** 1. It is NOT a backslash ('\'), OR
** 2. It is itself escaped, OR
** 3. It is between single quotes (where no characters are escapable), OR
** 4. It IS TRUE that the character was found between double quotes AND the next
** character is not escapable (double quotes admit only three escapable
** characters, which are '"', '$' and '\').
**
** The former was the original formulation, however I inverted the logic and
** switched to the latter becuase the former requires all conditions to be
** checked to come to a decision, whereas the latter only requires one of the
** checks to fail to come to a decision. This means the latter logic can be
** ordered from most likely to least likely failure conditions in order to save
** CPU cycles, which is what I have attempted to do. The double quote check is
** actually a composite of three checks, so it goes last.
*/

int				is_escape_char(char chr, char next_chr,
unsigned char escape_flag, unsigned char quote_flag)
{
	if (chr != '\\' || escape_flag || quote_flag == 2 ||
	(quote_flag == 1 && ft_memchr(DQUOTE_ESC_CHARS, next_chr, 3) == NULL))
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
** tries to save the location of its value to memory. When found, variable value
** locations are saved on a per-token basis in a linked list called var_list in
** the order of their appearance within the token.
**
** A character WILL be considered a variable if:
**
** 1. It is unescaped, AND
** 2. It is not between single quotes, AND
** 3. It is a '$', AND
** 4. The corresponding value location is successfully saved to memory by
** var_alloc.
**
** This is semantically the same as the inverse logic, saying that the character
** will NOT be considered a variable if:
**
** 1. It is NOT '$', OR
** 2. It is escaped, OR
** 3. It is between single quotes, OR
** 4. The var_alloc function FAILS to find a value for it (note, this is not the
** same as failing to find the variable, as non-existent variables WILL point to
** a null character - see var_alloc in env_var_handling.c for details).
**
** The former was the original formulation. The latter, with inverted logic, was
** implemented because it has exclusive conditions, ordered from most to least
** likely to be true, which should save CPU cycles. Normally, the first check
** should fail, and NULL should be returned.
**
** If the character is NOT considered a variable, a null pointer is returned. If
** the character IS considered a variable, the address of the character after
** the last character of its name will be returned by the function var_alloc.
** The function var_alloc will also return NULL if it fails.
*/

char			*is_variable(char chr, char *var_name, t_tokendata *tokdata, t_micli *micli)
{
	//if (!tokdata->escape_flag && tokdata->quote_flag != 2 && chr == '$' && var_alloc(var_name, micli)) //if unescacped, single quotes are not open and the '$' character is found
	if (chr != '$' || tokdata->escape_flag || tokdata->quote_flag == 2)
		return (NULL);
	return (var_alloc(var_name, micli)); //Try to find and save the address of the variable value
}

/*
** This function checks the character addressed passed as chr against the
** character address passed as end_var_addr to determine whether the character
** is the end of a variable name.
**
** The variable flag doubles as a pointer to the end of the name of the variable
** that it flagged.
**
** The character will NOT be considered the end of a variable name if:
**
** 1. The end_var_addr/variable flag is a NULL pointer, OR
** 2. The address of the character passed as chr is lower than the end_var_addr.
**
** The character IS thus considered to be the end of a variable when the chr
** address reaches the end_var_addr.
**
** The function returns 0 if the end of a variable name is not found and 1 if it
** is found. Commonly, when the variable flag is not set, there will only be a
** single null check at the beginning, and 0 will be returned.
*/

int				is_end_var_name(char *chr, char *end_var_addr)
{
	if (end_var_addr == NULL || chr < end_var_addr)
		return(0);
	return(1);
}

/*
** This function serves to process a character in a token. If the character is
** special, the appropriate flag is set to indicate it so special things can be
** done with it. ;) This function prepares characters in a token to be copied
** to a clean string for use in micli_argv, flagging characters for deletion or
** substitution as needed.
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
** ////VARIABLE CHECK\\\\
** - Is the character the end of a variable name?
**	The variable flag will be set to NULL.
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
** - Is the character substituting for a variable?
**	The variable flag will be set to indicate that subsequent characters are
**	part of the variable name. The character will be flagged as SUB to indicate
**	that it be substituted for the corresponding variable value. The token size
**	counter will be incremented by the size of the variable value. NOTE: A set
**	variable flag is a pointer to the last character in the variable name and
**	will be unset by the variable check at the beginning of the function when
**	that character is reached.
**
** - Is the character part of a variable name?
**	The character will be flagged for deletion.
**
** If none of the special instructions are applicable, then the general
** instructions for a generic character will be as follows:
**
** ////GENERAL INSTRUCTIONS (MUTUALLY EXCLUSIVE WITH SPECIAL INSTRUCTIONS)\\\\
** 1. Reset the escape flag (an active escape flag will prevent ANY special
** instructions from being executed, so general instructions will always be the
** ones applied to an escaped character).
** 2. Increment the token size counter to count the character.
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
	if (is_end_var_name(chr, micli->tokdata.var_flag))
		micli->tokdata.var_flag = NULL;
	if ((*chr == '>' || *chr == '<') && !micli->tokdata.escape_flag && !micli->tokdata.quote_flag) //If it's a redirect command
	{
		// if (*(chr + 1) == '>' && *chr == '>')
		// {
		// 	micli->cmdline.redir_out_flag = 2; //append mode
		// 	micli->tokdata.toksize += 2;
		// }
		// else
		// {
		// 	if (*chr == '>')
		// 		micli->cmdline.redir_out_flag = 1; //trunc mode
		// 	else
		// 		micli->cmdline.redir_in_flag = 1;
		// 	micli->tokdata.toksize += 1;
		// }
		if (*(chr + 1) == '>' && *chr == '>')
			micli->tokdata.toksize += 2;
		else
			micli->tokdata.toksize += 1;
		if (!micli->cmdline.redir_end) 
		{
			micli->cmdline.redir_start = chr;
			micli->cmdline.redir_end = find_redir_end(chr);
		}
	}
	else if (micli->cmdline.redir_end < chr) //If we've reached the end of a redirect instruction set, unset the flag
	{
		micli->cmdline.redir_start = NULL;
		micli->cmdline.redir_end = NULL;
	}
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
	else if (!micli->tokdata.var_flag && (micli->tokdata.var_flag = is_variable(*chr, (chr + 1), &micli->tokdata, micli)))
	{
		//micli->tokdata.var_flag = 1;
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