/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 21:17:37 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/13 18:26:32 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
** This function executes built-ins when they are called from shell.
*/

int	exec_builtin(char *cmd, t_micli *micli)
{
	if (!(ft_strcmp(cmd, "exit")))
		exit_success(micli);
	else if (!(ft_strcmp(cmd, "cd")))
		return (ft_cd((const char **)micli->cmdline.micli_argv, micli));
	else if (!(ft_strcmp(cmd, "pwd")))
		return (ft_pwd((const char**)micli->cmdline.micli_argv));
	else if (!(ft_strcmp(cmd, "echo")))
		return (ft_echo((const char **)micli->cmdline.micli_argv, micli));
	else if (!(ft_strcmp(cmd, "unset")))
		return (ft_unset(micli->cmdline.micli_argv, micli));
	else if (!(ft_strcmp(cmd, "env")))
		return (ft_env(micli->envp, (const char **)micli->cmdline.micli_argv));
	else if (!(ft_strcmp(cmd, "export")))
		return (ft_export((const char **)micli->cmdline.micli_argv, micli));
	return (128);
}

char	find_pos(const char *name, size_t name_len, char **envp)
{
	int	i;

	i = 0;
	while (envp[i] != NULL && ft_strncmp(name, envp[i], name_len))
		i++;
	return (i);
}

size_t	ft_name_len(const char *str)
{
	int	i;

	i = 0;
	while (str[i] != 0 && str[i] != '=')
		i++;
	return (i);
}

/*
** Modified to use a mask to sort the list in alphabetical order without
** changing the list. Use the returned mask to print the list in alphabetical
** order. -Miki
*/

size_t	*export_order(t_micli *micli)
{
	size_t *mask;
	size_t swap;
	size_t countarr;
	size_t i;
	size_t j;

	countarr = ft_countarr(micli->envp);
	mask = clean_calloc(countarr, sizeof(size_t), micli);
	printf("%zu\n", countarr);
	i = 0;
	while(i < countarr)
	{
		mask[i] = i;
		i++;
	}
	// z = 0;
	// while (z < countarr)
	// 	printf("MASK: %zu\n", mask[z++]);
	//order array in alphabetical reorder with bubble sort algo
	i = 0;
	while(i < countarr - 1)
	{
		j = 0;
		while (j < countarr - i - 1)
		{
			if(ft_strcmp(micli->envp[mask[j]], micli->envp[mask[j + 1]]) > 0)
			{
				swap = mask[j];
				mask[j] = mask[j + 1];
				mask[j + 1] = swap;
			}
			j++;
		}
		i++;
	}
	// i = 0;
	// 	while (mask[i] < countarr - 1)
	// 	{
	// 		printf("MASK NO: %zu, MASK ARR NO: %zu declare -x %s\n", mask[i], i, micli->envp[mask[i]]);
	// 		i++;
	// 	}
	return(mask);
}

// void	export_order(t_micli *micli)
// {
// 	int		i;
// 	int		j;
// 	char	countarr;
// 	char	*tmp_envp;

// 	i = 0;
// 	countarr = ft_countarr(micli->envp);
// 	while (i < countarr - 1)
// 	{
// 		j = 0;
// 		while (j < countarr - i - 1)
// 		{
// 			if (ft_strcmp(micli->envp[j], micli->envp[j + 1]) > 0)
// 			{
// 				tmp_envp = micli->envp[j];
// 				micli->envp[j] = micli->envp[j + 1];
// 				micli->envp[j + 1] = tmp_envp;
// 			}
// 			j++;
// 		}
// 		i++;
// 	}
// }
