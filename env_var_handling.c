/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_var_handling.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/05 15:55:31 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/26 21:36:04 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Returns the pointer to the first variable in envp with a key corresponding to
** the argument passed as name. If no key matches the name, NULL is returned.
**
** The function ft_strncmp returns true if a match to name is NOT found within
** first name_len characters of *ptr, and false (0) if a match to name IS found
** within the first name_len characters of *ptr. So, if the function returns
** false (0), it means there was a match, and since that is the pointer we want,
** we leave the while before it can be incremented again.
**
** HOWEVER, if the name is found within the first name_len characters of *ptr
** BUT the following character in *ptr is NOT '=' (name=), it is NOT considered
** a match. Thus, for example, searching for "HO" will only resolve to non-NULL
** if "HO=" exists, and NOT to "HOME=", because:
**
** 0 1 2 3 4
** H O M E =
**
** Where name_len is 2 (for HO), *ptr + name_len dereferences 'M', which is not
** '='.
**
** Hence, the following expression must be false to break the while loop:
**
** (ft_strncmp(name, *ptr, name_len || *(*ptr + name_len) != '=')
**
** That is to say, ft_strncmp must be false AND non-existence of '=' after the
** name must also be false (if EITHER is true, the while loop continues).
**
** Hope that is clear. It's really a compact little function, but does a lot. ;)
**
** NOTE: REVIEW PURPOSE OF $_ VARIABLE (NOT COMPLETED).
**
** NOTE: THE FT_PRINTF IS DEBUG CODE, REMOVE FROM FINAL VERSION.
*/

char	*find_var(const char *name, size_t name_len, char **envp)
{	
	char	**ptr;

	ptr = NULL;
	if (envp && (ptr = envp))
		while (ft_printf("PTR ADDR: %s\n", *ptr) && *ptr && (ft_strncmp(name, *ptr, name_len) || *(*ptr + name_len) != '='))
			ptr++;
	return(*ptr);
}

/*
** This function retrieves the length of the last resolved variable in a token
** so that enough memory can be reserved to substitute it in as needed.
*/

size_t	get_var_lengths(t_list *var_lst)
{
	size_t	len;

	while (var_lst->next)
		var_lst = var_lst->next;
	len = ft_strlen(var_lst->content);
	return (len);
}

/*
** This function tests for valid variable name characters. If 1 is returned, the
** character is valid within a variable name. If 2 is returned, the character is
** valid within a variable name as long as it is not the first character in the
** name. If 0 is returned, the character is not a valid variable name.
**
** https://www.gnu.org/software/bash/manual/html_node/Definitions.html#index-name
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
** This function resolves variable names to their associated values in the envp
** array and saves the addresses to the first character of those values within
** said array in a linked list.
**
** If a variable name is not found in envp, a pointer to '\0' is passed.
**
** Note the var_buffer is a list of pointers to addresses that are part of envp.
** During runtime, memory is only reserved for the list structures, NOT the
** values pointed to from within the structures.
**
** Since I don't reserve memory for list content at runtime, and therefore don't
** free it either, I point to a literal constant (NUL) for non-existent
** variables that need to be substituted for a null character, as NUL is
** likewise declared at compile-time in minishell.h and so does not need to be
** freed.
*/

void	var_buffer(char *var_name, size_t var_name_strlen, t_micli *micli)
{
	char	*varp;
	t_list	*new;		

	varp = find_var(var_name, var_name_strlen, micli->envp);
	//ft_printf("THIS IS ONLY A TEST OF FIND_VAR: %s\n", varp);
	if (varp)
	{
		varp = (ft_strchr(varp, '=') + 1); //Variable value starts at character after '='
		//ft_printf("VAR NAME CONTENT: %s\n", varp);
		new = ft_lstnew(varp);
	}
	else
		new = ft_lstnew(NUL); //If no match is found for a variable name among envp, resolve to a '\0' character.
	if (!micli->token->var_lst)
		micli->token->var_lst = new;
	else
		ft_lstadd_back(&micli->token->var_lst, new);
	
}

/*
** This function identifies variable names and saves them to a string so that
** they can later be resolved.
**
** Names are considered to end when invalid variable characters are
** found in the string following the initial '$'. The first character
** must be a letter or underscore. Remaining letters must be letters, numbers or
** an underscore. Ref:
**
** https://www.gnu.org/software/bash/manual/html_node/Definitions.html#index-name
**
** Once the extent of the variable name is determined and a copy of it is made,
** the copy's address is sent to the var_buffer function to find the associated
** variable in envp, for later insertion of the variable value into the
** corresponding token (substituting the memory addresses starting from where
** its '$' character was detected).
**
** Variable values inserted in place of unescaped '$' names in order of their
** appearance in the token. '$' characters without a valid name in front of them
** are treated as if escaped.
**
** NOTE: PRINTFS ARE DEBUG CODE; REMOVE FROM FINAL VERSION.
*/

int		var_alloc(char *var_name, t_micli *micli)
{
	size_t	i;
	int		name_state;
	char	*varnamecpy;

	i = 0;
	if ((name_state = isvarchar(var_name[i++])) != 1) //Initial varchar must be a letter or underscore https://www.gnu.org/software/bash/manual/html_node/Definitions.html#index-name
	{
		//ft_printf("INVALID VAR NAME: %.*s\n", i, var_name);
		return (0);
	}
	else
		while (var_name[i] && (name_state = isvarchar(var_name[i]))) //find end of varname. subsequent varchars may be alphanumeric or underscore.
			i++;
	//ft_printf("VAR NAME: %.*s\n", i, var_name); //var_name is var_name[0] --> var_name[i - 1];
	varnamecpy = clean_calloc(i + 1, sizeof(char), micli);
	//micli_cpy(varnamecpy, var_name, &var_name[i], micli);
	strncpy(varnamecpy, var_name, (&var_name[i] - var_name)); //need to make a strncpy :p 
	//ft_printf("COPIED VAR NAME: %s\n", varnamecpy);

	var_buffer(varnamecpy, i, micli);

	varnamecpy = ft_del(varnamecpy);
	return(1);
	
}
