/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_var_handling.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/05 15:55:31 by mrosario          #+#    #+#             */
/*   Updated: 2021/04/03 13:30:38 by mrosario         ###   ########.fr       */
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
*/

char	*find_var(const char *name, size_t name_len, char **envp)
{
	char	**ptr;

	ptr = NULL;
	if (envp)
	{
		ptr = envp;
		while (*ptr && (ft_strncmp(name, *ptr, name_len) \
		 || (*(*ptr + name_len) != '=' && *(*ptr + name_len) != '\0')))
			ptr++;
	}
	return (*ptr);
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
** www.gnu.org/software/bash/manual/html_node/Definitions.html#index-name
*/

int	isvarchar(char chr)
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
	if (*var_name == '?')
	{
		if (micli->cmd_result_str)
			micli->cmd_result_str = ft_del(micli->cmd_result_str);
		micli->cmd_result_str = ft_itoa(micli->cmd_result);
		new = ft_lstnew(micli->cmd_result_str);
	}
	else if (varp)
	{
		varp = (ft_strchr(varp, '='));
		if (varp)
			varp++;
		else
			varp = NUL;
		new = ft_lstnew(varp);
	}
	else
		new = ft_lstnew(NUL);
	if (!micli->token.var_lst)
		micli->token.var_lst = new;
	else
		ft_lstadd_back(&micli->token.var_lst, new);
}

/*
** This function identifies variable names and saves them to a string so that
** they can later be resolved.
**
** The function tests each character in the string passed as var_name to see
** whether it meets the criteria to be considered the end of the variable name.
**
** Variable names end when any character is found that cannot be a valid
** variable name according to the isvarchar function, OR when one of the special
** conditions are met (see below).
**
** -------------------------THE SPECIAL CONDITIONS------------------------------
** 1. The name begins with '?'.
**		A name beginning with ? is considered to refer to the exit status of the
**		last executed command and ends immediately. Micli saves the exit status
**		of the last executed command at micli->cmd_result.
** 2. The name begins with '$'.
**		A name beginning with $ is considered to refer to the PID of current
**		shell instance. It is reserved here, but STILL UNIMPLEMENTED. This is
**		not a subject requirement, so will implement this if we have time.
** 3. The name begins with '!', '@' or a digit (0-9).
**		Bash also seems to treat these characters as special, accepting them
**		(for scripting, maybe?) and always ending the variable name immediately
**		after they are found, regardless of what the next character is. Not sure
**		what they do, though, and would have to investigate.
** -----------------------------------------------------------------------------
**
** Once the extent of the variable name is determined and a copy of it is made,
** the copy's address is sent to the var_buffer function to find the associated
** variable value in envp, for later insertion of the variable value into the
** corresponding token (substituting the memory addresses starting from where
** its '$' character was detected). The declared token size will be increased by
** the length of the variable value so as to acommodate it.
**
** Variable values are inserted in place of unescaped '$' charcters in order of
** their appearance in the token. '$' characters without a valid name in front
** of them are treated as if escaped. Note that special conditions are
** considered valid names.
**
** Once the variable has been resolved by var_buffer, the copy of the variable
** name is freed.
**
** If a variable name is found and resolved, this function returns the address
** of the first character after the last character in the variable name. If a
** valid variable name is not found, a NULL pointer is returned.
*/

char	*var_alloc(char *var_name, t_micli *micli)
{
	size_t	i;
	char	*varnamecpy;

	i = 0;
	if (var_name[i] == '?' || var_name[i] == '$' || var_name[i] == '!' \
	 || var_name[i] == '@' || ft_isdigit(var_name[i]))
		i++;
	else if (!isvarchar(var_name[i++]))
		return (NULL);
	else
		while (isvarchar(var_name[i]))
			i++;
	varnamecpy = clean_calloc(i + 1, sizeof(char), micli);
	ft_strncpy(varnamecpy, var_name, (&var_name[i] - var_name));
	var_buffer(varnamecpy, i, micli);
	varnamecpy = ft_del(varnamecpy);
	return (&var_name[i]);
}
