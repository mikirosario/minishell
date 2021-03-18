/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_execution.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 19:33:19 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/17 22:42:52 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function finds comma-separated builtins in the BUILTIN constant string
** declared in the minishell.h header. If the argument passed as cmd matches a
** comma-separated builtin within the BUILTIN constant string, 1 is returned.
** Otherwise, 0 is returned. We need to compare against the string length of
** cmd, so since the BUILTIN string indicators are comma as well as
** null-terminated, we first check to make sure cmd is exactly the same length
** as the proposed built-in match before bothering to compare it.
**
** I'm assuming that after the pointer subtraction, size_t should be enough to
** hold the result until we become transhuman and routinely input 64 bit command
** names.
*/

int		find_builtin(char *cmd)
{
	char	*startl;
	char	*endl;

	startl = BUILTINS;
	endl = startl;
	while (*startl)
	{
		while (*endl && *endl != ',')
			endl++;
		if (ft_strlen(cmd) == (size_t)(endl - startl) \
		&& !(ft_strncmp(startl, cmd, endl - startl)))
			return (1);
		else if (*endl == ',')
			endl++;
		startl = endl;
	}
	return (0);
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
** points to (micli->cmdline.cmd).
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
	DIR				*dir;
	struct dirent	*dirent;
	char			*ret;
	size_t			y;

	ret = NULL;
	y = 0;
	if (find_builtin(cmd))
		return (cmd);
	micli->tokdata.path_array = clean_ft_split(&paths[5], ':', micli);
	while (!ret && micli->tokdata.path_array[y])
	{
		dir = opendir(micli->tokdata.path_array[y]);
		if (dir)
			while((dirent = readdir(dir)))
				if (!(ft_strncmp(dirent->d_name, cmd, ft_strlen(cmd) + 1)))
					ret = generate_pathname(micli->tokdata.path_array[y], \
					cmd, micli);
		if (dir)
			closedir(dir);
		y++;
	}
	micli->tokdata.path_array = free_split(micli->tokdata.path_array);	
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
** save the result in the char pointer array micli->cmdline.micli_argv. This
** array contains the command name (cmd) as the first pointer, and each argument
** to the command as the remaining pointers, in the same order as they were
** input into shell.
**
** We search for the PATH environmental variable in the envp pointer array. Once
** we locate it, we send its address and the address of the command name to
** the function find_cmd_path.
**
** We need to find the exec_path - that is, the location of the binary we are
** going to use to execute the command. If the command is a built-in, this will
** be a function within minishell instead, as we haven't separated the built-ins
** into their own binaries, sadly.
**
** We analyse the first characters of the command name to determine whether it
** is itself an absolute or relative path. If so, the exec_path will be the same
** as the command.
**
** If the command is not a path, the function find_cmd_path will be used. If the
** command is among the shell built-ins, the address pointed to by cmd will be
** returned. If it is not, a binary matching the command name will be searched
** among the PATH directories and, if one is found, the path to it will be
** stored in a string and the address of that string will be returned. Note in
** this case exec_path will have to be freed later. If neither a built-in nor a
** PATH binary matches the command name, a NULL pointer is returned.
**
** Thus, if there is a directory path to the executable, it will be in exec_path
** If the command is among the builtins, exec_path will just point back to cmd,
** and the builtin pointer is also set, so that we can tell that we have a
** builtin later on.
**
** As in bash, 127 is returned as the command result if the command could not be
** resolved at all.
**
** Depending on the built-in that is called, we may have to execute it within
** parent process, rather than within a child. This is the case for cd, unset,
** exit and export when called with arguments (that is, when it sets variables),
** not when it prints them. So there is a chonky conditional expression to
** determine whether any of this is true. If it is, we use exec_builtin, which
** is the same function we use to launch built-ins in children, we just launch
** it directly in the parent.
**
** If the preceding conditional expression is NOT true, then we will want to
** launch in a child process, and we use exec_child_process for that, unless
** exec_path is NULL, which would mean we could not resolve the command.
**
** In any case, we store the process termination status in the variable
** micli->cmd_result. The function exec_child_process handles this internally.
** The cmd_result can then be displayed by the user with the $? variable.
**
** If 127 is returned it signifies that the command could not be found or the
** path to the binary did not work. Thus, if the command cannot be resolved, we
** set this directly, and if the execve fails in the child, we exit(127) to have
** it set by the calling function (which is exec_child_process).
**
** In either case, if cmd_result is detected as 127, then if exec_path is NULL
** we return a command not found error, and otherwise we return strerror(2) for
** the path error.
**
** If we are inside a pipeline, for every command we try to execute, we need to
** increment the pipes.cmd_index counter, crucial to controlling the little
** beasties. The clear_pipes function, triggered on pipe termination, now resets
** the counter along with the rest of the pipes struct.
**
** If a pipeline has been requested by the user, then the command will be
** launched in special pipeline mode and the parent will NOT wait for any child
** except the last child in the pipeline. The variable micli->pipes.array_size
** contains the number of file descriptors. The variable micli->pipes.count
** contains the number of pipes detected in the original line. The variable
** micli->pipes.cmd_index tracks the current command, starting with command 0.
*/

void	exec_cmd(char *cmd, t_list *arglst, t_micli *micli)
{
	char			*exec_path;
	char			*builtin;
	char			*path_var;

	exec_path = NULL;
	builtin = NULL;
	micli->cmdline.micli_argv = create_micli_argv(cmd, arglst, micli);
	path_var = find_var("PATH", 4, micli->envp);

	if (*cmd == '/' || (*cmd == '.' && *(cmd + 1) == '/') || (*cmd == '.' && \
	*(cmd + 1) == '.' && *(cmd + 2) == '/') || (*cmd == '~' && *(cmd + 1) == '/'))
		exec_path = cmd;
	else if ((exec_path = find_cmd_path(cmd, path_var, micli)) == cmd) //if cmd is not path look in builtins (if cmd is builtin, return cmd), if cmd is not builtin look in PATH (return path), otherwise return NULL export without arguments should be launched as child...) //if cmd is not path look in builtins (if builtin, return cmd), if cmd is not builtin look in PATH (return path), otherwise return NULL, if export is executed without arguments do not run as builtin
		builtin = cmd;
	if (!exec_path)
		micli->cmd_result = 127;
	if (builtin != NULL && ((!ft_strcmp(exec_path, "export") && micli->cmdline.micli_argv[1] != NULL) \
	|| !ft_strcmp(exec_path, "exit") || !ft_strcmp(exec_path, "cd") || !ft_strcmp(exec_path, "unset")))
		micli->cmd_result = exec_builtin(builtin, micli);
	else if (exec_path != NULL) //if cmd is a path or a builtin or has been found in PATH variable it is not null, otherwise it is NULL.
		exec_child_process(exec_path, builtin, cmd, micli);
	//printf("DEBUG INFO\nCMD NO.: %zu\nLAST CMD RESULT: %d\nLAST PIPED CHILD RESULT: %i\nPIPE_FAIL ARRAY RESULT: %zu\n\n", micli->pipes.cmd_index, micli->cmd_result, child_res, micli->pipes.pipe_fail[micli->pipes.cmd_index]);
	if (micli->cmd_result == 127)
	{
		if (!exec_path)
			ft_printf("micli: %s: command not found\n", cmd);
		else
			ft_printf("micli: %s: %s\n", cmd, strerror(2));
	}
	if (micli->pipe_flag)
		micli->pipes.cmd_index++; //increment cmd_index for child pipe_count comparison
	// else if (micli->pipes.cmd_index) //clear_pipes debería encargarse de esto ahora
	// 	micli->pipes.cmd_index = 0;
}
