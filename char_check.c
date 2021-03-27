/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   char_check.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/14 18:01:19 by miki              #+#    #+#             */
/*   Updated: 2021/03/27 21:28:35 by mrosario         ###   ########.fr       */
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

int	is_escape_char(char chr, char next_chr,
unsigned char escape_flag, unsigned char quote_flag)
{
	if (chr != '\\' || escape_flag || quote_flag == 2 \
	 || (quote_flag == 1 && ft_memchr(DQUOTE_ESC_CHARS, next_chr, 3) == NULL))
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
**
** To determine whether or not a character is quoted, and, if so, by what kind
** of quotes, we use the quote_flag, which contains this information.
**
** The quote_flag is a two bit unsigned integer that codifies this information
** in the following format:
**	Bit1   Bit2
** single:double
**
** Bin Dec
** 00  (0) = All quotes closed.
** 01  (1) = Double quotes opened, single quotes closed.
** 10  (2) = Single quotes opened, double quotes closed.
** 11  (3) = Double quotes opened, single quotes opened.
*/

int	is_quote_char(char chr, unsigned char escape_flag, \
unsigned char quote_flag)
{
	if ((!escape_flag \
	 && ((quote_flag == 0 && (chr == '"' || chr == '\'')) \
	 || (quote_flag == 1 && chr == '"'))) \
	 || (quote_flag == 2 && chr == '\''))
		return (1);
	return (0);
}

/*
** This function tests a character in a string to see whether it meets the
** criteria to be considered the start of a variable. This also depends on the
** result of the var_alloc function, which determines whether the variable can
** be located and tries to save the location of its value to memory. When found,
** variable value locations are saved on a per-token basis in a linked list
** called var_list in the order of their appearance within the token.
**
** A character WILL be considered the start of a variable if:
**
** 1. It is unescaped, AND
** 2. It is not between single quotes, AND
** 3. It is a '$', AND
** 4. It is not part of a variable name (var_flag has not already been set).
** 5. The corresponding value location is successfully saved to memory by
** var_alloc.
**
** This is semantically the same as the inverse logic, saying that the character
** will NOT be considered the start of a variable if:
**
** 1. It is NOT '$', OR
** 2. It is part of a variable name (var_flag has already been set), OR
** 3. It is escaped, OR
** 4. It is between single quotes, OR
** 5. The var_alloc function FAILS to find a value for it (note, this is not the
** same as failing to find the VARIABLE, as non-existent variables WILL point to
** a null character - see var_alloc in env_var_handling.c for details).
**
** The former was the original formulation. The latter, with inverted logic, was
** implemented because it has exclusive conditions, ordered from most to least
** likely to be true, which should save CPU cycles. Normally, the first check
** should fail, and NULL should be returned.
**
** If the character is NOT considered the start of a variable, a null pointer is
** returned. If the character IS considered a variable, the address of the
** character after the last character of its name will be returned by the
** function var_alloc. The function var_alloc will also return NULL if it fails.
*/

int	is_variable_start(char chr, t_tokendata *tokdata)
{
	if (chr != '$' || tokdata->var_flag || tokdata->escape_flag \
	 || tokdata->quote_flag == 2)
		return (0);
	return (1);
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

int	is_variable_end(char *chr, char *end_var_addr)
{
	if (end_var_addr == NULL || chr < end_var_addr)
		return (0);
	return (1);
}

/*
** This function returns true if the detected character is a the first character
** in a new set of redirect instructions.
**
** A character will be considered the first character of a new set of redirect
** instructions if it is either a '<' or a '>' and no redirect_end address has
** yet been specified (redirect_end == NULL) and it is unescaped and unquoted.
** Fairly straightforward.
*/

int	is_redirect_start(char chr, unsigned char escape_flag, \
unsigned char quote_flag, char *redir_end)
{
	if ((chr == '>' || chr == '<') && redir_end == NULL && !escape_flag \
	 && !quote_flag)
		return (1);
	return (0);
}
