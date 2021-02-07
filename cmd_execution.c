/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_execution.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 19:33:19 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/08 00:38:26 by miki             ###   ########.fr       */
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
** This function generates the pathname of a binary after it is found within the
** binary directories included in the PATH environmental variable, which can
** then be passed to execve to execute the binary.
**
** The path to the directory is saved at the address pointed to by the path
** pointer without a trailing /, so the trailing / must be added in manually.
** That is why I created this function instead of just using str_join. :p
**
** The command name is saved at the address pointed to by the cmd pointer.
**
** First we determine the lengths of path and cmd. Then we reserve enough memory
** to hold both path and cmd, plus 2 bytes, one for the trailing / we need to
** put between path/cmd, and another for the terminating null.
**
** Then we copy path to the reserved memory block, add a '/' after path, and
** concatenate it with cmd. We return the result, which will be the pathname.
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
** This function finds the path where the binary specified by cmd exists from
** among the directories specified within the PATH environmental variable. The
** The PATH variable is passed as the const char pointer paths.
**
** The first five characters of the PATH variable are PATH=, so we start
** searching from paths[5]:
**
**	0 1 2 3 4 5 start at pos 5
**	P A T H = / ...
**
** Directories in the PATH variable are divided by ':', so we use ft_split to
** isolate each directory in its own null-terminated string.
**
** First we compare cmd with a hardcoded constant string literal, BUILTINS,
** defined (currently) in minishell.h (might put this in a config file later).
**
** If cmd is in the BUILTIN string, we return the same address as the one cmd
** points to (micli->token->cmd).
**
** If cmd is not found in BUILTIN we open all the directories in path_array one
** by one and check every entry in each directory one by one. If any of these
** entries matches cmd, we assemble a path name using the path where it was
** found (generate_pathname does this) and return an address to the path name
** thus generated.
**
** If no match is found either in BUILTINS or in the PATH directories, we return
** NULL.
*/

char	*find_cmd_path(char *cmd, const char *paths, t_micli *micli)
{
	char			**path_array;
	DIR				*dir;
	struct dirent	*dirent;
	char			*ret;
	size_t			y;

	ret = NULL;
	path_array = clean_ft_split(&paths[5], ':', micli);	
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

/*
** This function attempts to execute a command passed to shell via the command
** line, with all of its arguments.
**
** First we create an argv array for the command using create_micli_argv and
** save the result in the char pointer array micli->token->micli_argv. This
** array contains the command name (cmd) as the first pointer, and each argument
** to the command as the remaining pointers, in the same order as they were
** input into shell.
**
** We search for the PATH environmental variable in the envp pointer array. Once
** we locate it, we send its address and the address of the command name to
** the function find_cmd_path.
**
** The function find_cmd_path will return the path to the binary whose name
** matches the command name among the directories listed in PATH. If the command
** is among the shell built-ins, the address pointed to by cmd will be returned
** instead. If no match is found either among the built-ins or the PATH
** directories, a NULL pointer is returned.
**
** Depending on the value returned, if we have a built-in we go to the
** exec_builtin function to execute it, if we have a binary we fork the program
** and call execve in the child function, sending it the pathname that we
** assembled with find_cmd_path, and if we haven't found anything we leave the
** function without doing anything.
**
** If a binary is executed in a child process we use waitpid to stop the parent
** from executing until the child function has terminated. We store the child
** process termination status in the variable micli->cmd_result. (NEED TO LINK THIS TO THE '?' ENVIRONMENTAL VARIABLE, WHEN CREATED)
**
** (ctrl-C, ctrl-D, ctrl-\ TENDRÍAN QUE DETENER EL CHILD PROCESS, NO EL PARENT!!!!!!)
**
** If find_cmd_path has to reserve memory to store an assembled pathname, the memory is freed.
*/

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