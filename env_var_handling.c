/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_var_handling.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/05 15:55:31 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/08 20:50:46 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function sums up the lengths of all resolved variables in a set of
** tokens so that enough memory can be reserved to substitute them in as needed.
*/

size_t	get_var_lengths(t_list *var_lst)
{
	size_t	len;

	while (var_lst)
	{
		len = ft_strlen(var_lst->content);
		var_lst = var_lst->next;
	}
	return (len);
}

/*
** This function tests for valid variable name characters. If 1 is returned, the
** character is valid within a variable name. If 2 is returned, the character is
** valid within a variable name as long as it is not the first character in the
** name. If 0 is returned, the character is not a valid variable name.
*/

int		isvarchar(char chr)
{
	if (ft_isalpha(chr) || chr == '_')
		return (1);
	else if (ft_isdigit(chr))
		return (2);
	return (0);
}

/*
** This function resolves variable names to their values in the envp array and
** saves the addresses of the beginning of those values in a linked list.
**
** All variable strings are terminated by ETX (ASCII 3), except the last one,
** which is null terminated.
*/

void	var_buffer(char *var_name, size_t var_name_strlen, t_micli *micli)
{
	int		y;
	int		x;
	t_list	*new;		

	y = 0;
	x = 0;
	while (micli->envp[y] && ft_strncmp(var_name, micli->envp[y], var_name_strlen))
		y++;
	if (micli->envp[y]) //need to parse this line, find the content, save it to an array or list or something, and fill it in...
	{
		//ft_printf("VAR NAME CONTENT: %s\n", micli->envp[y]);
		while (micli->envp[y][x] && micli->envp[y][x] != '=')
			x++;
		ft_printf("VAR NAME CONTENT: %s\n", &micli->envp[y][x + 1]);
		new = ft_lstnew(&micli->envp[y][x + 1]);
	}
	else
	{
		while (micli->envp[0][x])
			x++;
		new = ft_lstnew(&micli->envp[0][x]);
	}
	if (!micli->token->var_lst)
		micli->token->var_lst = new;
	else
		ft_lstadd_back(&micli->token->var_lst, new);
	
}

/*
** This function identifies variable names, saves them to a string and uses the
** libft function strncmp to locate them in envp so that they can be resolved.
**
** Names are considered to end when invalid variable characters are
** found in the string following the initial '$'. The first character
** must be a letter or underscore. Remaining letters must be letters, numbers or
** an underscore. Ref:
**
** https://www.gnu.org/software/bash/manual/html_node/Definitions.html#index-name
**
** Once the extent of the variable name is determined and a copy of it is made,
** the copy's address is sent to the var_buffer function to add the content to
** the variable buffer, for later insertion into the corresponding token
** (substituting memory addresses from its $ character).
**
** Variable names are inserted in place of unescaped $ names in order of
** appearance. $ characters without a valid name in front of them are treated as
** if escaped.
*/

int		var_alloc(char *var_name, t_micli *micli)
{
	size_t	i;
	int		name_state;
	char	*varnamecpy;

	(void)micli;
	i = 0;
	if ((name_state = isvarchar(var_name[i++])) != 1) //Initial varchar must be a letter or underscore https://www.gnu.org/software/bash/manual/html_node/Definitions.html#index-name
	{
		ft_printf("INVALID VAR NAME: %.*s\n", i, var_name);
		return (0);
	}
	else
		while (var_name[i] && (name_state = isvarchar(var_name[i]))) //find end of varname. subsequent varchars may be alphanumeric or underscore.
			i++;
	ft_printf("VAR NAME: %.*s\n", i, var_name); //var_name is var_name[0] --> var_name[i - 1];
	varnamecpy = clean_calloc(i + 1, sizeof(char), micli);
	//micli_cpy(varnamecpy, var_name, &var_name[i], micli);
	strncpy(varnamecpy, var_name, (&var_name[i] - var_name)); //need to make a strncpy :p 
	ft_printf("COPIED VAR NAME: %s\n", varnamecpy);

	var_buffer(varnamecpy, i, micli);

	varnamecpy = ft_del(varnamecpy);
	return(1);
	
}


// varname_alloc(char chr, t_micli *micli)
// {
// 	static size_t	namelen = 0;
// 	static size_t	bufsize = READLINE_BUFSIZE;

// 	if (!micli->token->varnames)
// 	{
// 		micli->token->varnames = clean_calloc(bufsize, sizeof(char), micli);
// 		namelen = 0;
// 		bufsize = READLINE_BUFSIZE;
// 	}
// 	else if (namelen >= READLINE_BUFSIZE)
// 	{
// 		bufsize += READLINE_BUFSIZE;
// 		if (!(micli->token->varnames = ft_realloc(micli->token->varnames, bufsize, micli)))
// 			exit_failure(micli);
// 	}
// 	micli->token->varnames[namelen] = chr;
// 	namelen++;
// 	if (!chr) //if chr is NULL, name ends here
// 		var_alloc(micli->token->varnames, micli);
// }


	//SACAR PATH DE SU CARCEL EN ENVP
	// int i = 0;
	// while (ft_strncmp(envp[i], "PATH", 4))
	// 	i++;
	// printf("%s\n", envp[i]);