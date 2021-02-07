/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_var_handling.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/05 15:55:31 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/07 20:48:03 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function tests for valid variable name characters. If 1 is returned, the
** character is valid within a variable name. If 2 is returned, the character is
** valid within a variable name as long as it is not the first character in the
** name. If 0 is returned, the character is not a valid variable name.
*/

int		isvarchar(char chr)
{
	if (isalpha(chr) || chr == '_')
		return (1);
	else if (isdigit(chr))
		return (2);
	return (0);
}

/*
** This functin resolves variable names and saves the result.
*/

/*
** This function identifies and saves variable names so that they can be
** resolved.
**
** Names are considered to end when invalid variable characters are
** found in the string following the initial '$'. The first character
** must be a letter or underscore.
**
** https://www.gnu.org/software/bash/manual/html_node/Definitions.html#index-name
*/

int		var_alloc(char *var_name, t_micli *micli)
{
	size_t	i;
	size_t	f;
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
	micli_cpy(varnamecpy, var_name, &var_name[i], DEL);
	ft_printf("COPIED VAR NAME: %s\n", varnamecpy);

	f = 0;
	while (micli->envp[f] && ft_strncmp(varnamecpy, micli->envp[f], i))
		f++;
	if (micli->envp[f]) //need to parse this line, find the content, save it to an array or list or something, and fill it in... if the content has its own variables they need to be resolved also...
		ft_printf("VAR NAME CONTENT: %s\n", micli->envp[f]);
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