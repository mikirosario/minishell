/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_execution.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/04 19:33:19 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/18 20:29:55 by mrosario         ###   ########.fr       */
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
	char	*path_var;
	int		stat_loc;
	pid_t	pid;

	
	exec_path = NULL;
	micli->cmdline.micli_argv = create_micli_argv(cmd, arglst, micli);
	
	// i = 0;
	// while (micli->cmdline.micli_argv[i])
	// 	ft_printf("%s\n", micli->cmdline.micli_argv[i++]);
	

	path_var = find_var("PATH", 4, micli->envp);
	
	//printf("%s\n", path_var);

	exec_path = find_cmd_path(cmd, path_var, micli);
	if (exec_path)
	{
		if (exec_path == cmd) //if find_cmd_path return value points to the same destination as the original cmd pointer, it means this command was found among the builtins and will be executed as a builtin
			micli->cmd_result = exec_builtin(exec_path, micli); //function must return exit status of executed builtin
		else
		{
			micli->pipe_reset_flag = micli->pipe_reset_flag == 2 ? 0 : micli->pipe_reset_flag + 1; //If the pipe reset flag reaches 3, reset to 0.
			if (micli->pipe_reset_flag == 0) // If this flag is 0, we read from 4, write to 1
			{
				close(micli->pipe[2]); // Close readpd 2
				pipe(&micli->pipe[2]);
			}
			else if (micli->pipe_reset_flag == 1) //If this flag is 1, we read from 0, write to 3
			{
				close(micli->pipe[4]); //Close readpd 4
				pipe(&micli->pipe[4]);
			}
			else								//If this flag is 2, we read from 2, write to 5
			{
				close(micli->pipe[0]); //Close readpd 0
				pipe(&micli->pipe[0]);
			}
			if (!micli->pipe_flag && !(pid = fork())) //unpiped child
				execve(exec_path, micli->cmdline.micli_argv, micli->envp);
			// else if (micli->pipe_flag && !(pid = fork()))//experimental piped child
			// {
			// 	if (micli->pipe_flag && !(pid = fork())) //unpiped child
			// 		execve(exec_path, micli->cmdline.micli_argv, micli->envp);	//temporary deactivation pipes
			// }
			else if (micli->pipe_flag && !(pid = fork())) //piped child
			{
				int writefd_pos;
				int readfd_pos;
				int i;

				i = 0;

				if (micli->pipe_reset_flag == 0)
				{
					writefd_pos = 1;
					readfd_pos = 4;
				}
				else if (micli->pipe_reset_flag == 1)
				{
					writefd_pos = 3;
					readfd_pos = 0;
				}
				else
				{
					writefd_pos = 5;
					readfd_pos = 2;
				}
				//close stdout (1), normally used for write to terminal, and make a duplicate
				// of fd[1], write end of pipe, and assign file descriptor 1 to it.
				if (ft_isbitset(micli->pipe_flag, 0)) //if micli->pipe_flag rightmost bit is set: 11 == read-write or 01 == write-only
					dup2(micli->pipe[writefd_pos], 1);
				//close stdin (0) normally used for read from terminal, and make a duplicate
				//of fd[0], read end of pipe, and assign file descriptor 0 to it.
				if (ft_isbitset(micli->pipe_flag, 1)) //if micli->pipeflag leftmost bit is set: 11 == read-write or 10 == read-only
					dup2(micli->pipe[readfd_pos], 0);
				while (i < 6)
					close(micli->pipe[i++]);
				execve(exec_path, micli->cmdline.micli_argv, micli->envp);//now execute command... =_=
			}
			if (micli->pipe_flag)
			{
				printf("PIPE FLAG: %u\nPIPE RESET FLAG: %u\n", micli->pipe_flag, micli->pipe_reset_flag);
				//micli->pipe_flag = 0;
				if (micli->pipe_reset_flag == 0) // If this flag is 0, we read from 4, write to 1
					close(micli->pipe[5]); // Close writepd 5
				else if (micli->pipe_reset_flag == 1) //If this flag is 1, we read from 0, write to 3
					close(micli->pipe[1]); //Close writepd 1
				else								//If this flag is 2, we read from 2, write to 5
				{
					ft_printf("I CAME HERE\n");
					close(micli->pipe[3]); //Close writepd 3s
				}
			}
			waitpid(pid, &stat_loc, WUNTRACED);
			micli->cmd_result = WEXITSTATUS(stat_loc);
			exec_path = ft_del(exec_path);
		}
	}
	else
	{
		micli->cmd_result = 127;
		ft_printf("%s: command not found\n", cmd);
	}
}