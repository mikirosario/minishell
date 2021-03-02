/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp_dup.c                                  		:+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 12:13:45 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/21 17:24:19 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// size_t	prune_undef_variables(char **envp)
// {
// 	size_t i;
// 	size_t len;

// 	i = 0;
// 	len = 0;
// 	while(envp[i])
// 		if (ft_strchr(envp[i++], '='))
// 			len++;
// 	return (len);
// }

/*
** This function counts the pointers in a NULL-terminated pointer array.
*/

size_t	ft_countarr(char **envp)
{
	size_t i;

	i = 0;
	while(envp[i])
		i++;
	return(i);
}

/*
** This function duplicates a character pointer array along with all of the
** content pointed to by each pointed, and is used to clone the envp array.
**
** The function assumes that the array and content are null-terminated.
*/

// char		**ft_envdup(char **envp, t_micli *micli)
// {
// 	size_t	envp_size;
// 	size_t	i;
// 	size_t	j;
// 	char	**new_envp;

// 	envp_size = ft_countarr(envp);
// 	i = envp_size;
// 	while(--i)
// 	{
// 		if (!(ft_strchr(envp[i], '=')))
// 			envp_size--;
// 	}
// 	//new_envp = malloc(sizeof(char *) * envp_size + 1);
// 	new_envp = clean_calloc(envp_size + 1, sizeof(char *), micli);
// 	i = 0;
// 	j = 0;
// 	while (i < envp_size)
// 	{
// 		if (ft_strchr(envp[j], '='))
// 		{
// 			new_envp[i] = clean_ft_strdup(envp[j], micli);
// 			i++;
// 		}
// 		j++;
// 	}
// 	return (new_envp);
// }

char		**ft_envdup(char **envp, t_micli *micli)
{
	size_t	envp_size;
	size_t	i;
	size_t	j;
	char	**new_envp;

	envp_size = ft_countarr(envp);
	i = envp_size;
	while(--i)
	{
		if (!(ft_strchr(envp[i], '=')))
			envp_size--;
	}
	//new_envp = malloc(sizeof(char *) * envp_size + 1);
	new_envp = clean_calloc(envp_size + 1, sizeof(char *), micli);
	i = 0;
	j = 0;
	while (i < envp_size)
	{
		if (ft_strchr(envp[j], '='))
		{
			new_envp[i] = clean_ft_strdup(envp[j], micli);
			i++;
		}
		j++;
	}
	return (new_envp);
}