/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_raw_line.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 12:20:47 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/17 05:15:12 by miki             ###   ########.fr       */
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
** This function accepts the address of the start of a new command line and
** returns the address of the end of the same command line to the calling
** function.
**
** Everything in the while (is_cmdline) loop is part of a single, continuous
** command line, and so the loop will continue until it discovers the end of the
** command line, which may be ';', signifying new command, '|', signifying new
** piped command, or '\0', signifying '\n' or EOF.
**
** The pointer lindex is used to iterate through the line from lstart until a
** ';', '|' or '\0'' (meaning '\n' or EOF) is found, at which point the fragment
** from lstart until the ';', '|' or NULL is passed to the process_cmdline
** function for tokenization. If an escape flag is detected at the beginning of
** the loop, we advance the index to analyse the character after it and set the
** flag to 1 to indicate the fact.
**
** Characters that are escaped or quoted will not count as the end of a command
** line, except, of course, the '\0' as we don't do multiline commands.
**
** NOTE:
** Note that in this particular implementation of the escape flag it isn't
** necessary to do an escape flag check in is_escape_char, because it will
** always return 0, but the check was designed for compatibility with a prior,
** less efficient escape flag routine I wrote that does require this check. So
** don't touch it, you'll break everything. ;)
*/

char	*find_cmdline_end(char *lindex)
{
	unsigned char quote_flag;
	unsigned char escape_flag;

	quote_flag = 0;
	escape_flag = 0;
	while (is_cmdline(*lindex, quote_flag, escape_flag))
	{
		escape_flag = 0;
		if (is_escape_char(*lindex, *(lindex + 1), escape_flag, quote_flag))
		{
			lindex++;
			escape_flag = 1;
		}
		if (!escape_flag)
		{
			if (*lindex == '|' || *lindex == ';')
				break ;
			quote_flag = toggle_quote_flag(*lindex, quote_flag);
		}
		lindex++;
	}
	return (lindex);
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
** If the last command line ended in ';' we might be at the start of a new
** pipeline. For example: "echo 2 | cat; echo 6 | cat | less". If pipes are then
** detected in the new command line, we are definitely at the start of a new
** pipeline. If that is the case, we do a pipe_reset to prepare the pipeline
** ahead of time with a bit of pre-processing.
**
** After a command line is processed, we advance one character and skip any
** subsequence spaces to retrieve the beginning of the next command line,
** assuming that we aren't going to be interested in the last character of the
** line (';' or '|'). Of course, we only do this if the character is not '\0'.
**
** The cmdline_end variable is a 'ñapa' to save the last preceding end of a
** command line so that we know whether we need to check for a new pipeline.
** Thus, since at the beginning of a raw line we always need to check, it starts
** initialized at ';'. Could do this with
** if (*lindex == ';' || lindex == &line[0]) too, but that's two checks instead
** of a char assignment and a check. :/ Don't judge me. :p
**
** Test functionality with this sequence:
** echo 2*2 | bc | cat; echo this \| "is |" 'a |' test; echo 2+3 | bc 
** Should return:
** 4
** this | is | a | test
** 5
** echo 2*5 > in; echo "="; bc<in>out; echo 2*10>in; echo "="; bc <in >>out;
** cat out
** Should return:
** =
** =
** 10
** 20
*/

void	process_raw_line(char *line, t_micli *micli)
{
	char			*lstart;
	char			*lindex;
	char			cmdline_end;

	if (!syntax_check(line))
		return ;
	lindex = line;
	cmdline_end = ';';
	while (*lindex)
	{
		lstart = lindex;
		if (cmdline_end == ';' && (micli->pipes.count = pipe_count(lstart)))
			pipe_reset(&micli->pipes, micli);
		lindex = find_cmdline_end(lindex);
		process_cmdline(lstart, lindex, micli);
		if ((cmdline_end = *lindex))
			lindex++;
		lindex = ft_skipspaces(lindex);
	}
}
