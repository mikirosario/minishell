/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_execution.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 19:33:19 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/30 19:33:42 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function prints the error message for commands that are not found. For
** commands provided as a path, it means they were not found in the specified
** location, which is strerror(2). For commands passed by name, it means they
** were not found among the built-ins (preferably) or among the binaries in
** PATH directories, from left to right (secondarily).
**
** If exec_path is NULL, the missing executable was passed as a command,
** otherwise it was passed as a path.
*/

void	print_not_found(char *exec_path, char *cmd)
{
	if (!exec_path)
		printf("micli: %s: command not found\n", cmd);
	else
		printf("micli: %s: %s\n", cmd, strerror(2));
}

/*
** This function determines whether a command should be executed in the parent
** (true) or in a child (false). It's less intimidating than it might look.
**
** A command should be executed in the parent if:
**
** 1. It is a built-in, AND
** 2. It is "export" with at least one argument, OR
** 3. It is "exit", OR
** 4. It is "cd", OR
** 5. It is "unset"
*/

int	exec_local(char *exec_path, char *builtin, t_micli *micli)
{
	if (builtin != NULL && ((!ft_strcmp(exec_path, "export") \
	 && micli->cmdline.micli_argv[1] != NULL) || !ft_strcmp(exec_path, "exit") \
	 || !ft_strcmp(exec_path, "cd") || !ft_strcmp(exec_path, "unset")))
		return (1);
	return (0);
}

/*
** This function determines whether a command is or is not a path.
**
** A command is a path if it begins with '/', './', '../' or '~/'.
*/

int	is_path(char *cmd)
{
	if (*cmd == '/' || (*cmd == '.' && *(cmd + 1) == '/') \
	 || (*cmd == '.' && *(cmd + 1) == '.' && *(cmd + 2) == '/') \
	 || (*cmd == '~' && *(cmd + 1) == '/'))
		return (1);
	return (0);
}

/*
** This function is another Norminette special. It checks to see if the
** exec_path pointer returned by find_cmd_path was pointed to cmd, indicating
** that it was found as a builtin. If it was found as abuiltin, the builtin
** flag is set by pointing builtin to cmd. Otherwise it is left NULL. *sigh*
*/

char	*builtin_check(char *exec_path, char *cmd)
{
	char	*builtin;

	builtin = NULL;
	if (exec_path == cmd)
		builtin = cmd;
	return (builtin);
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
** NOTE: The code
**
** else
**	{
**		exec_path = find_cmd_path(cmd, path_var, micli);
**		builtin = builtin_check(exec_path, cmd);
**	}
**
** Replaces a check that, before assignments in control structures were
** banned, just looked like this:
**
** else if ((exec_path = find_cmd_path(cmd, path_var, micli)) == cmd)
**		builtin = cmd;
**
** I even need the function to avoid running over the 25 line limit. Anyway...
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
** The clear_pipes command resets the cmd_index counter as needed.
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
	if (is_path(cmd))
		exec_path = cmd;
	else
	{
		exec_path = find_cmd_path(cmd, path_var, micli);
		builtin = builtin_check(exec_path, cmd);
	}
	if (!exec_path)
		micli->cmd_result = 127;
	if (exec_local(exec_path, builtin, micli))
		micli->cmd_result = exec_builtin(builtin, micli);
	else if (exec_path != NULL)
		exec_child_process(exec_path, builtin, cmd, micli);
	if (micli->cmd_result == 127)
		print_not_found(exec_path, cmd);
	if (micli->pipe_flag)
		micli->pipes.cmd_index++;
}
