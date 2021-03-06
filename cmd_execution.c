/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_execution.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 19:33:19 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/06 19:12:26 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function finds comma-separated builtins in the BUILTIN constant string
** declared in the minishell.h header. If the argument passed as cmd matches a
** comma-separated builtin within the BUILTIN constant string, 1 is returned.
** Otherwise, 0 is returned.
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
		if ((!(strncmp(startl, cmd, endl - startl))))
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
		ret = cmd;
	else
	{
		micli->tokdata.path_array = clean_ft_split(&paths[5], ':', micli);	//	0 1 2 3 4 5
																			//	P A T H = / ... start at pos 5
		while (!ret && micli->tokdata.path_array[y]) //for every dir in PATH
		{
			dir = opendir(micli->tokdata.path_array[y]); //open dir
			//ft_printf("%s\n", path_array[y]);
			if (dir)
				while((dirent = readdir(dir)))  //go through every dir entry
					if (!(ft_strcmp(dirent->d_name, cmd))) //stop if entry coincides with cmd
						ret = generate_pathname(micli->tokdata.path_array[y], cmd, micli); //concatenate dir path with command name
					//ft_printf("%s\n", dirent->d_name);
			if (dir)
				closedir(dir);
			y++;
		}
		micli->tokdata.path_array = free_split(micli->tokdata.path_array);	
	}

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
** If a command is executed in a child process we use waitpid to stop the parent
** from executing until the child function has terminated. We store the child
** process termination status in the variable micli->cmd_result. This can be
** displayed by the user with the $? variable.
**
** --- PIPES ---
**
** If a pipeline has been requested by the user, then the command will be
** launched in special pipeline mode and the parent will NOT wait for any child
** except the last child in the pipeline. The variable micli->pipes.array_size
** contains the number of file descriptors. The variable micli->pipes.count
** contains the number of pipes detected in the original line. The variable
** micli->pipes.index keeps track of the current pipe, starting with pipe0.
**
** Pipeline mode uses the micli->pipe_flag to determine the command's read/write
** status. The command may be launched as write-only (first command in the
** pipeline), read-only (last command in the pipeline) or read-write (any
** command sandwiched between commands in the pipeline). 1 == write-only, 2 ==
** read-only 3 == read-write.
**
** Depending on the read/write status, the child will duplicate the read and/or
** write file descriptors of the preceding and/or current pipe, respectively,
** assigning stdin or stdout to the duplicates through the dup2 function.
**
** All of the pipes.array file descriptors IN THE CHILD are then closed to
** eliminate their references as counted in the associated file structs.
**
** When pipes.count - pipes.index == 0, we have reached the last piped command.
**
**					 -------->pipes.count == 3<--------
**					 ^				  ^				  ^
**					 ^  	pipe0	  ^		pipe1	  ^		pipe2
**					 |	[write][read] |	[write][read] | [write][read]
**			   stdout	1			0	3			2	5			4	stdin
**					↓	↑	   		↓	↑	  		↓	↑			↓	↑
**				  	cmd1			cmd2			cmd3			cmd4
**					child1			child2			child3			child4
**	pipes.index	==	0	----------> 1	---------->	2	---------->	3
**	pipes.count ==  3											  - 3
**																	↓
**																	0 == end cmd
**
** If we have reached the last command, all of the pipe file descriptors in the
** parent's process are closed and the parent waits for the last child to
** terminate. Unlike bash, only the last child's exit status is saved, but this
** technically isn't *pipe* functionality, but *exit status* functionality. :p
** If you REALLY want me to malloc an exit status array I'll do it, but I think
** it would kind of be wasting time. ;p
**
** (ctrl-C, ctrl-D, ctrl-\ TENDRÍAN QUE DETENER EL CHILD PROCESS, NO EL PARENT!!!!!!)
**
** If find_cmd_path had to reserve memory to store an assembled pathname, the memory is freed.
**
** LAST ADDITIONAL PIPE UNNECESSARY????????
*/

void	exec_cmd(char *cmd, t_list *arglst, t_micli *micli)
{
	char	*exec_path;
	char	*builtin;
	char	*path_var;
	int		stat_loc;
	size_t	i;

	pid_t	pid;

	i = 0;
	exec_path = NULL;
	builtin = NULL;
	micli->cmdline.micli_argv = create_micli_argv(cmd, arglst, micli);
	
	// i = 0;
	// while (micli->cmdline.micli_argv[i])
	// 	ft_printf("%s\n", micli->cmdline.micli_argv[i++]);
	

	path_var = find_var("PATH", 4, micli->envp);
	
	//printf("%s\n", path_var);
	
	if (*cmd == '/' || (*cmd == '.' && *(cmd + 1) == '/') || (*cmd == '.' && *(cmd + 1) == '.' && *(cmd + 2) == '/') || (*cmd == '~' && *(cmd + 1) == '/')) //if ispath
		exec_path = cmd; //exec path is cmd if cmd is path
	else if ((exec_path = find_cmd_path(cmd, path_var, micli)) == cmd) //if cmd is not path look in builtins (if builtin, return cmd), if cmd is not builtin look in PATH (return path), otherwise return NULL
		builtin = cmd;
	if (exec_path != NULL) //if cmd is a path or a builtin or has been found in PATH variable it is not null, otherwise it is NULL.
	{
		if (builtin != NULL) //if builtin is defined, command is a builtin, después guarreo para evitar que al liberar exec_path se libere memoria apuntada por cmd antes de tiempo :p 
			micli->cmd_result = exec_builtin(builtin, micli); //function must return exit status of executed builtin (piped builtins not yet functional)
		else //otherwise it's a path, send the path to execve.
		{
			if (!micli->pipe_flag && !(pid = fork())) //unpiped child
				execve(exec_path, micli->cmdline.micli_argv, micli->envp);
			else if (micli->pipe_flag && !(pid = fork())) //piped child inherits micli->pipes.array[]
			{
				int writefd_pos;
				int readfd_pos;

				//readfd_pos = micli->pipes.index * 2; //Equation to derive appropriate read fd in the pipe array from the pipe number
				//writefd_pos = readfd_pos + 3; //Equation to derive apropriate write fd from the read fd
				writefd_pos = micli->pipes.index * 2 + 1; //New equation to derive appropriate write fd in the pipe array from the pipe number, with no need for initial phantom pipe
				readfd_pos = writefd_pos - 3; //Causes sign overflow for first pipe, but the first pipe in a set should always leave pipeflag in 01 state, so we should never use the overflow value... please give me a break on this one, compiler, I know what I'm doing... :p
				//printf("PIPE FLAG: %u\nPIPE INDEX: %zu\nREAD FD: %d\nWRITE FD: %d\n", micli->pipe_flag, micli->pipes.index, readfd_pos, writefd_pos);
				//close stdout (1), normally used for write to terminal, and make a duplicate
				// of fd[1], write end of pipe, and assign file descriptor 1 to it.
				if (ft_isbitset(micli->pipe_flag, 0)) //if micli->pipe_flag rightmost bit is set: 11 == read-write or 01 == write-only
					dup2(micli->pipes.array[writefd_pos], STDOUT_FILENO); //stdout == 1
				//close stdin (0) normally used for read from terminal, and make a duplicate
				//of fd[0], read end of pipe, and assign file descriptor 0 to it.
				if (ft_isbitset(micli->pipe_flag, 1)) //if micli->pipeflag leftmost bit is set: 11 == read-write or 10 == read-only
					dup2(micli->pipes.array[readfd_pos], STDIN_FILENO); //stdin == 0;
				while (i < micli->pipes.array_size) //there are array_size fds (2 fds per pipe)
					close(micli->pipes.array[i++]);
				execve(exec_path, micli->cmdline.micli_argv, micli->envp);//now execute command... =_=
			}
			if (!micli->pipe_flag || (micli->pipes.count - micli->pipes.index == 0))
			{	
				while (i < micli->pipes.array_size) //there are array_size fds (2 fds per pipe)
					close(micli->pipes.array[i++]);
				waitpid(pid, &stat_loc, WUNTRACED);
				micli->cmd_result = WEXITSTATUS(stat_loc);
				micli->pipe_flag = 0; //Reset pipe_flag
				//printf("CMD RESULT: %d\n", micli->cmd_result);
			}
			else if (micli->pipe_flag)
			{

				micli->pipes.index++; //increment pipe_index for child pipe_count comparison
			}
			if (exec_path != cmd) //guarreo para evitar liberar memoria apuntada por cmd antes de tiempo provocando un double free :p
				exec_path = ft_del(exec_path);
		}
	}
	else
	{
		micli->cmd_result = 127;
		ft_printf("%s: command not found\n", cmd);
	}
}

//Sequential else

// else
// 		{
// 			micli->pipe_reset_flag = micli->pipe_reset_flag == 2 ? 0 : micli->pipe_reset_flag + 1; //If the pipe reset flag reaches 3, reset to 0.
// 			pipe_reset(micli->pipe_reset_flag, micli->pipe); //Si devuelve 0, ha fallado... devuelve ese error tipo "NO PUEDORRRRL MÁS PIPES"
// 			if (!micli->pipe_flag && !(pid = fork())) //unpiped child
// 				execve(exec_path, micli->cmdline.micli_argv, micli->envp);
// 			else if (micli->pipe_flag && !(pid = fork())) //piped child
// 			{
// 				int writefd_pos;
// 				int readfd_pos;
// 				int i;

// 				i = 0;

// 				pipe_selector(micli->pipe_reset_flag, &writefd_pos, &readfd_pos);
// 				//close stdout (1), normally used for write to terminal, and make a duplicate
// 				// of fd[1], write end of pipe, and assign file descriptor 1 to it.
// 				if (ft_isbitset(micli->pipe_flag, 0)) //if micli->pipe_flag rightmost bit is set: 11 == read-write or 01 == write-only
// 					dup2(micli->pipe[writefd_pos], 1);
// 				//close stdin (0) normally used for read from terminal, and make a duplicate
// 				//of fd[0], read end of pipe, and assign file descriptor 0 to it.
// 				if (ft_isbitset(micli->pipe_flag, 1)) //if micli->pipeflag leftmost bit is set: 11 == read-write or 10 == read-only
// 					dup2(micli->pipe[readfd_pos], 0);
// 				while (i < 6)
// 					close(micli->pipe[i++]);
// 				execve(exec_path, micli->cmdline.micli_argv, micli->envp);//now execute command... =_=
// 			}
// 			if (micli->pipe_flag)
// 			{
// 				printf("PIPE FLAG: %u\nPIPE RESET FLAG: %u\n", micli->pipe_flag, micli->pipe_reset_flag);
// 				close_write_end_preceding_pipe(micli->pipe_reset_flag, micli->pipe);
// 			}
// 			waitpid(pid, &stat_loc, WUNTRACED);
// 			micli->cmd_result = WEXITSTATUS(stat_loc);
// 			exec_path = ft_del(exec_path);
// 		}