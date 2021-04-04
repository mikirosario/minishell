/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   iamerror.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/08 19:25:04 by mrosario          #+#    #+#             */
/*   Updated: 2021/04/04 23:41:46 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** The command is valid if:
**
** It exists (cmd && *cmd), AND
** It is not '.' OR '..',
** The *path_var exists, AND
** The character after PATH in the *path_var is an '='.
**
** If the path_var is valid we increment the path_var pointer to point to the
** character after the '=' in PATH=.
*/

int	is_valid_command(char *cmd, char **path_var)
{
	if (cmd && *cmd && !(*cmd == '.' && ((*(cmd + 1) == '\0') \
	 || (*(cmd + 1) == '.' && *(cmd + 2) == '\0'))) && *path_var \
	 && *(*path_var + 4) == '=')
	 {
		*path_var += 5;
		return (1);
	 }
	return (0);
}

/*
** In case of a system error, the error is printed. Otherwise, a generic unknown
** error is printed.
*/

void	sys_error(t_micli *micli)
{
	if (micli->syserror)
		printf("%s\n", strerror(micli->syserror));
	else
		printf("General error\n");
}

/*
** Prints the string passed as error_message and the first two characters of the
** string passed as error_location, unless the second character is '\0', in
** which case only the first character is printed. In the event of a syntax
** error, command result is set to 2, which appears to be bash behaviour.
*/

int	print_error(char *error_message, char *error_location, t_micli *micli)
{
	if (!error_location)
		printf("💥 %s\n", error_message);
	else if (error_location[1])
		printf("💥 %s '%.2s'\n", error_message, error_location);
	else
		printf("💥 %s '%.1s'\n", error_message, error_location);
	micli->cmd_result = 2;
	return (0);
}

/*
** We have an invalid redirect if after finding a redirect command and skipping
** any spaces we find another redirect (except in the case of '>>') or a command
** line end ('|'), (';') or ('\0'). These are considered syntax errors in
** minishell. Some of these, like <>, are actually valid in bash, but minishell
** does not implement them per the subject.
**
** If the character is not an invalid redirect, 0 is returned. Otherwise, a
** syntax error is printed and 1 is returned.
*/

int	invalid_redir(char *line, unsigned char quote_flag, \
unsigned char esc_flag, t_micli *micli)
{
	if ((*line != '<' && *line != '>') || quote_flag || esc_flag)
		return (0);
	if (*line == '>' && *(line + 1) == '>')
		line++;
	line = ft_skipspaces(++line);
	if (!*line || *line == '<' || *line == '>' || *line == '|' || *line == ';')
	{
		print_error(SYN_ERROR, line, micli);
		return (1);
	}
	return (0);
}

/*
** This function parses the whole line checking for the following syntax errors:
**
** 1. First character means end of command line (';' or '|').
** 2. Between a character meaning end of command line and the following command
** line there are more characters meaning end of command line, i.e.: ls;;ls or
** ls;|ls.
**
** Characters enclosed by quotes are not considered to mean end of command line.
** To keep track of this we use a quote_flag that is toggled when quotes are
** found. This is a simpler check than the token parser proper as any open
** quotes will designify the end of command line characters.
**
** If a syntax error is found we print an error message and return 0. Otherwise
** we return 1.
**
** NOTE: Treating '||' as a syntax error is not exactly bash behaviour, as bash
** implements '||' as meaning execute next command only if preceding command
** returned false, but the minishell subject does not require us to implement
** this feature, so I just treat it as a syntax error.
**
** NOTE: The same goes for '<>' and '<<<'. In real bash it is valid, but it's
** not a feature in minishell.
**
** Norminette made me do it.
*/

int	syntax_check(char *line, t_micli *micli)
{
	unsigned char	quote_flag;
	unsigned char	esc_flag;

	quote_flag = 0;
	line = ft_skipspaces(line);
	if (*line == ';' || *line == '|' || *line == '<' || *line == '>')
		return (print_error(SYN_ERROR, line, micli));
	while (*line)
	{
		esc_flag = 0;
		if (is_escape_char(*line, *(line + 1), esc_flag, quote_flag) && line++)
			esc_flag = 1;
		if (is_quote_char(*line, esc_flag, quote_flag))
			quote_flag = toggle_quote_flag(*line, quote_flag);
		if ((*line == '|' || *line == ';') && !quote_flag && !esc_flag)
		{
			line = ft_skipspaces(++line);
			if (*line == '|' || *line == ';')
				return (print_error(SYN_ERROR, line, micli));
		}
		if (invalid_redir(line, quote_flag, esc_flag, micli))
			return (0);
		line++;
	}
	return (1);
}
