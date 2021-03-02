/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/01 20:47:05 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/02 21:25:44 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


int		is_redirect_instruction(const char *chr, t_tokendata *tokdata, t_cmdline *cmdline)
{
	if (!tokdata->quote_flag && !tokdata->escape_flag && (*chr == '>' || *chr == '<'))
	{
		tokdata->redirect_flag = chr;
		if (*chr == '>' && *(chr + 1) == '>')
			cmdline->redir_out_flag = 2; //append
		else if (*chr == '>')
			cmdline->redir_out_flag = 1; //truncate
	}
}

char	*retrieve_redirect_instructions(char *redir_str, t_cmdline *cmdline)
{
	int				*redir_fd;
	unsigned char	escape_flag;

	escape_flag = 0;
	//Condiciones de salida:
	//(!*redir_str || !escape_flag && (*redir_str == ';' || *redir_str == '|' || *redir_str == ' ') )
	
	while (!(!*redir_str || !escape_flag && (*redir_str == ';' || *redir_str == '|' || *redir_str == ' '))) //Mientras las condiciones de salida no sean ciertas
	{
		if (*redir_str == '<')
			redir_fd = cmdline->fd_redir_in;
		else if (*redir_str == '>')
			redir_fd = cmdline->fd_redir_out;
		cmd
		
	}
	
}

char	*get_redir_fds(char *filename, t_micli *micli)
{
	
}