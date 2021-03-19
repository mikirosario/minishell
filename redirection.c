/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/01 20:47:05 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/19 20:22:47 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function will attempt to open a file at the path found in the redirect
** instructions. The file name will be copied directly from the token to a
** region of memory of toksize size using micli_cpy, which copies from tok_start
** to tok_end.
**
** The mode in which it opens the requested file will depend on the preceding
** instruction flag, set with the interpret_redir_instruction function.
**
** The file descriptor of the opened file will be saved to fd_redir_out for
** files opened for writing, and fd_redir_in for files opened for reading.
**
** If a file was already opened for the indicating (write or read channel), it
** will be closed before a new one is opened.
**
** The files will all be opened with permissions set to 644.
**
** The open flags to be used are preset in the FILE_WRITE_TR, FILE_WRITE_AP and
** FILE_READ defines.
*/

void	open_redir_file(t_normis_fault *tonti, t_micli *micli)
{
	char	*dst;

	dst = clean_calloc(micli->tokdata.toksize, sizeof(char), micli);
	micli_cpy(dst, micli->tokdata.tok_start, micli->tokdata.tok_end, micli);
	if (micli->cmdline.redir_out_flag)
	{
		if (micli->cmdline.fd_redir_out)
			close(micli->cmdline.fd_redir_out);
		if (micli->cmdline.redir_out_flag == 1)
			micli->cmdline.fd_redir_out = open(dst, tonti->f_tr, tonti->perms);
		else
			micli->cmdline.fd_redir_out = open(dst, tonti->f_ap, tonti->perms);
	}
	else if (micli->cmdline.redir_in_flag)
	{
		if (micli->cmdline.redir_in)
			close(micli->cmdline.fd_redir_in);
		micli->cmdline.fd_redir_in = open(dst, tonti->f_re, tonti->perms);
	}
	dst = ft_del(dst);
	micli->cmdline.redir_out_flag = 0;
	micli->cmdline.redir_in_flag = 0;
}

/*
** This function will read a redirect instruction and set the corresponding flag
** depending on whether it signals a write append, write truncate or read
** instruction. Each command line can have a maximum of one write and one read
** instruction, so if more than one instruction is received for a command line
** the last one will be overwritten.
**
** The instructions are ">>" for write append, ">" for write truncate, and "<"
** for read.
**
** The redir_out_flag will be set to 2 for write append or 1 for write truncate.
** The redir_in_flag will be set to 1 for read. Both flags will be initialized
** at 0 for no instructions.
*/

void	interpret_redir_operator(const char *redir, t_micli *micli)
{
	if (*redir == '>' && *(redir + 1) == '>')
		micli->cmdline.redir_out_flag = 2;
	else
	{
		if (*redir == '>')
			micli->cmdline.redir_out_flag = 1;
		else
			micli->cmdline.redir_in_flag = 1;
	}
}

/*
** The purpose of this function is to find the end of a set of redirect
** instructions within a command line string, which we save as a reference to
** unset the redirect flag that causes the process_token function to sequester
** redirect instructions from the argument list so that they are not sent to the
** command as arguments.
**
** Yes, it's complicated. Yes, I did redirects last. xD
**
** Redirect strings are considered to end when the redirect flag is set and an
** unescaped redirect command is NOT found after the last file path is
** processed. File paths are considered to end on unescaped spaces, end of
** cmdline (unescaped ';' or '|'), or null meaning EOL/EOF.
**
**			 set	     unset
**		 redir_flag	  redir_flag
**			  ↑			   ↑
** cmd arg arg<file > file arg arg
**			  ↓			   ↓
**			start		  stop
**			redir		  redir
**			sequester	  sequester
**
** Thus the conditions under which we will consider that a redirect end has been
** found are:
**
** 1. A null character is found, OR
** 2. The escape flag is NOT set, AND
**		a. A '>', '<', ' ', ';' or '|' is found.
**
** The inverse of this logic are the conditions for considering that a redirect
** end has NOT been found, that is, that the while within which we are searching
** for the redirect end character should CONTINUE. The inverse logic is:
**
** 1. The character is non-null, AND
** 2. The escape flag IS set, OR,
**		a. Neither '>' nor '<' nor ' ' nor ';' nor '|' is found.
**
** NOTE: Syntax errors, like '>>>' or '>> ;' are checked before this in the
** syntax_check function, so we should only reach this function with strings
** that are syntax-perfect.
*/

char	*find_redir_end(char *redir_str)
{
	unsigned char	escape_flag;

	escape_flag = 0;
	while (*redir_str == '>' || *redir_str == '<')
	{
		if (*redir_str == '>' && *(redir_str + 1) == '>')
			redir_str += 2;
		else
			redir_str += 1;
		redir_str = ft_skipspaces(redir_str);
		while (*redir_str && (escape_flag || (*redir_str != '>' \
		 && *redir_str != '<' && *redir_str != ' ' && *redir_str != ';' \
		 && *redir_str != '|')))
		{
			if (!escape_flag && *redir_str == '\\')
				escape_flag = 1;
			else
				escape_flag = 0;
			redir_str++;
		}
		redir_str = ft_skipspaces(redir_str);
	}
	return (redir_str);
}
