/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_execution.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 19:33:19 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/07 23:43:12 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


/*
** This function executes built-ins when they are called from shell.
*/

void	exec_builtin(char *cmd, t_micli *micli)
{
	if (!(ft_strcmp(cmd, "exit")))
		exit_success(micli);
}

/*
**
*/

char	*generate_pathname(char *path, char *cmd, t_micli *micli)
{
	char	*ret;
	size_t	pathlen;
	size_t	cmdlen;

	pathlen = ft_strlen(path);
	cmdlen = ft_strlen(cmd);
	ret = clean_calloc(pathlen + cmdlen + 2, sizeof(char), micli);
	ft_memcpy(ret, path, pathlen);
	ret[pathlen] = '/';
	ft_strlcat(ret, cmd, pathlen + cmdlen + 2);
	//ft_printf("GREAT SUCCESS: %s\n", ret);
	return(ret);
}


/*
**
*/

char	*find_cmd_path(char *cmd, const char *paths, t_micli *micli)
{
	char			**path_array;
	DIR				*dir;
	struct dirent	*dirent;
	char			*ret;
	size_t			y;

	ret = NULL;
	path_array = clean_ft_split(&paths[5], ':', micli);	//	0 1 2 3 4 5
														//	P A T H = / ... start at pos 5
	y = 0;
	if (ft_strnstr(BUILTINS, cmd, micli->builtin_strlen))
		ret = cmd;
	else
		while (!ret && path_array[y]) //for every dir in PATH
		{
			dir = opendir(path_array[y]); //open dir
			//ft_printf("%s\n", path_array[y]);
			if (dir)
				while((dirent = readdir(dir)))  //go through every dir entry
					if (!(ft_strcmp(dirent->d_name, cmd))) //stop if entry coincides with cmd
						ret = generate_pathname(path_array[y], cmd, micli); //concatenate dir path with command name
					//ft_printf("%s\n", dirent->d_name);
			closedir(dir);
			y++;
		}			
	
	path_array = free_split(path_array);
	return (ret);
}

/*
** This function reserves memory for and copies the memory addresses to an array
** of pointers wherein array[0] points to the command name, array[end] points to
** NULL, and all pointers in between point to the successive command arguments.
**
** Memory is reserved for micli->tokdata.args + 2 pointers, that is, for all
** arguments plus the command plus the null pointer. This array can be passed to
** execve.
**
** The free_token function will free memory reserved for this array.
*/

char	**create_micli_argv(char *cmd, t_list *arglst, t_micli *micli)
{
	char	**argv;
	size_t	i = 0;

	argv = clean_calloc(micli->tokdata.args + 2, sizeof(char *), micli);
	while (i < micli->tokdata.args + 2)
	{
		if (!i)
			argv[i++] = cmd;
		else
		{
			if (arglst)
			{
				argv[i++] = arglst->content;
				arglst = arglst->next;
			}
			else
				argv[i++] = NULL;
		}
	}
	return (argv);
}

void	exec_cmd(char *cmd, t_list *arglst, t_micli *micli)
{
	char	*exec_path;
	int		stat_loc;
	int		i;
	pid_t	pid;

	
	exec_path = NULL;
	micli->token->micli_argv = create_micli_argv(cmd, arglst, micli);
	
	// i = 0;
	// while (micli->token->micli_argv[i])
	// 	ft_printf("%s\n", micli->token->micli_argv[i++]);
	
	i = 0;
	while (ft_strncmp(micli->envp[i], "PATH", 4))
		i++;
	
	//printf("%s\n", micli->envp[i]);

	exec_path = find_cmd_path(cmd, micli->envp[i], micli);
	if (exec_path)
	{
		if (exec_path == cmd) //if find_cmd_path return value points to the same destination as the original cmd pointer, it means this command was found among the builtins and will be executed as a builtin
			exec_builtin(exec_path, micli);
		else
		{
			if (!(pid = fork()))
				execve(exec_path, micli->token->micli_argv, micli->envp);
			waitpid(pid, &stat_loc, WUNTRACED);
			micli->cmd_result = WEXITSTATUS(stat_loc);
			// ft_printf("STAT_LOC: %d\n", micli->cmd_result);
			exec_path = ft_del(exec_path);
		}
	}
}