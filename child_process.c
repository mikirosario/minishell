/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/14 21:17:29 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/26 02:18:43 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function will set the in and out variables to the file from or to which
** the child process's command must read or write, that is to say, they will be
** used to replace stdin and stdout if set here.
**
** The first step is to determine whether or not the child is part of a pipeline
** This can be determined by checking micli->pipe_flag. If it is part of a
** pipeline, we will have an array of pipes to choose from. The position in this
** array of the pipe that we want to use for writing and/or reading in this
** child depends on the command number within the pipeline. The command number
** can be checked at micli->pipes.cmd_index. The following equations will give
** the appropriate pipe number:
**
** writefd_pos = micli->pipes.cmd_index * 2 + 1.
** readfd_pos = writefd_pos - 3.
**
** How does that work? Read on to find out...
**
** 							--- PIPES ---
**
** Pipes at the beginning of a pipeline will need only a writefd and pipes at
** the end of a pipeline will need only a readfd. Pipes in the middle of a
** pipeline will need both a readfd (from the preceding pipe) and a writefd (to
** the current pipe).
**
** Visualize this as follows.
**
** For the example commands "echo 2*10|cat|bc|less", the pipes.array looks like
** this:
**
** 				pipe0			pipe1			pipe2
**			   0	 1		   2	 3		   4	 5
** pipes	[read][write]	[read][write]	[read][write]
**
** Four commands, three pipes. Thus, the pipeline will look like this:
**
**					 -------->pipes.count == 3<--------
**					 ^				  ^				  ^
**					 ^  	pipe0	  ^		pipe1	  ^		pipe2
**					 |	[write][read] |	[write][read] | [write][read]
**			   stdin	1			0	3			2	5			4	stdout
**					↓	↑	   		↓	↑	  		↓	↑			↓	↑
**				  	cmd0			cmd1			cmd2			cmd3
**					child0			child1			child2			child3
**	cmd_index	==	0	----------> 1	---------->	2	---------->	3
**	pipes.count	==  3											  - 3
**																	↓
**																	0 == end cmd
**
** Some example situations within this pipeline:
**
** If we are in child2:
** 	The cmd_index will be 2. Thus the writefd_pos will be 2 * 2 + 1 = 5, and
** 	the read_fd pos will be 5 - 3 = 2. Thus, we will read from pipe1 and write
** 	to pipe2.
**
** If we are in child3:
** 	The cmd_index will be 3. Thus, the writefd_pos will be 3 * 2 + 1 = 7, and
**	the read_fd pos will be 7 - 3 = 4. Because this is the last pipe in the
**	pipeline, the pipe_flag will be 10 (binary) == 2 (decimal), which will stop
**	the write_fd from being used, so no overflow. Thus, we will read from pipe2
**	and write to stdout.
**
** If we are in child0:
**	The cmd_index will be 0. Thus, the writefd_pos will be 0 * 2 + 1 = 1, and
**	the read_fd pos will be 1 - 3 = SIZE_MAX - 1 (because it's a size_t and
**	unsigned). Because this is the first pipe in the pipeline, the pipe_flag
**	will be 01 (binary) == 1 (decimal), which will stop read_fd from being
**	used, so no overflow. Thus, we will read from stdin and write to pipe0.
**
** Once we have the right positions in the pipes.array, we simply save the fds
** at those positions to in and out.
**
** 								PIPE_FLAG
**
** The pipe_flag is a two bit integer that codifies which fd is needed as
** follows:
**
** BINARY DECIMAL
**
** 	00 		0 		= No pipe.
** 	01 		1		= first pipe, read from stdin, write to pipe
** 	10 		2 		= last pipe, read from pipe, write to stdout
** 	11 		3 		= middle pipe, read from and write to pipe
**
** Pipeline mode uses the micli->pipe_flag to determine the command's read/write
** status. The command may be launched as write-only (first command in the
** pipeline), read-only (last command in the pipeline) or read-write (any
** command sandwiched between commands in the pipeline). 1 == write-only, 2 ==
** read-only 3 == read-write.
**
** And that's it. :)
**
**							--- REDIRECTS ---
**
** The next step is to determine whether redirects have been requested. As in
** bash, if both a redirect and pipe have been requested, the redirect has
** precedence. Thus, for example, if a redirect to file has been requested,
** 'out' will be set to the file's fd, otherwise, if a pipe has been requested,
** it will be set to the pipe fd at writefd_pos. The same will be done for 'in'.
**
** Redirects are simpler affair at this stage. If a command needs to redirect to
** a file, the appropriate fd will be saved at micli->cmdline.fd_redir_out, and
** if it needs to redirect from a file, the appropriate fd will be saved at
** micli->cmdline.fd_redir_in. These instructions have precedence over piping
** instructions, as per bash, so if they exist we just directly copy them to in
** or out, as appropriate, and use them.
**
** NOTE: Thus DOES assume the convention that redirect will never give 0 as file
** fd. That is because we're assuming that it starts out reserved for stdin.
*/

void	get_new_stdin_stdout(int *in, int *out, t_micli *micli)
{
	size_t	writefd_pos;
	size_t	readfd_pos;

	if (micli->pipe_flag)
	{
		writefd_pos = micli->pipes.cmd_index * 2 + 1;
		readfd_pos = writefd_pos - 3;
	}
	if (micli->cmdline.fd_redir_out)
		*out = micli->cmdline.fd_redir_out;
	else if (ft_isbitset(micli->pipe_flag, 0))
		*out = micli->pipes.array[writefd_pos];
	if (micli->cmdline.fd_redir_in)
		*in = micli->cmdline.fd_redir_in;
	else if (ft_isbitset(micli->pipe_flag, 1))
		*in = micli->pipes.array[readfd_pos];
}

/*
** This function will execute a command as a builtin if it is a buitin, or a
** binary if it is a binary. If a builtin fails, we exit with exit_failure, and
** if it succeeds we exit with exit_success, to notify the parent of the result
** via waitpid. The execve function handles this internally.
**
** I could free everything except argv and envp before launching execve, but
** honestly... there are no lost references, and it's going to be freed when the
** process exits anyway... give me a break. :p
**
** Realizing that I needed an exit_failure condition after execve in case execve
** fails took me much longer than I feel comfortable with. xD
*/

void	child_process_exec(char *builtin, char *exec_path, t_micli *micli)
{
	int	res;

	if (builtin == NULL)
	{
		execve(exec_path, micli->cmdline.micli_argv, micli->envp);
		exit(127);
	}
	else
	{
		res = exec_builtin(exec_path, micli);
		if (!res)
		{
			freeme(micli);
			exit(EXIT_SUCCESS);
		}
		else
		{
			freeme(micli);
			exit(EXIT_FAILURE);
		}
	}
}

/*
** Once spawned, a child process must first determine whether it will use stdin
** and stdout, or replace them with another file (a redirect or a pipe),
** depending on the information supplied by the parent.
**
** Depending on this information, the child may duplicate the read and/or write
** file descriptors of provided files and assign stdin or stdout to the
** duplicates through the dup2 function to redirect any input/output that would
** normally come from or go to these file to the provided files. This function
** is performed by the get_new_stdin_stdout function. Detailed information on
** the logic used is provided in its description.
**
** All of the pipes.array file descriptors IN THE CHILD are then closed to
** eliminate their references as counted in the associated file structs. More on
** this below.
**
** Once we've determined whether input or output will be diverted from
** stdin/stdout to another file (pipes are actually just files), if we need to
** divert either output or input, we use dup2 to close stdout/stdin and
** duplicate the file we want to divert to/from. This will now point the fd
** number of the closed fd to same file struct as the duplicated fd.
**
** So take, for example, dup2(out, STDOUT_FILENO).
**
** This will first close STDOUT_FILENO, and then assign that number to the file
** pointed to by out, so within the child process STDOUT_FILENO will now point
** to out, like so (at least as I currently understand it):
** ______________________________________________________________
** ______________________________________________________________
**
**							BEFORE DUP2:
** 	  fd tables			fd struct	inode struct	file data
** parent_out 	== 3►►►▼
** child_out  	== 3►►►►inode_ptr►►►filedata_ptr►►►►myfile
**						refs == 2	refs == 1
** """"""""""""""""""""""""""""""""""""""""""""""""""""""""""
** child_stdout == 1►►►►inode_ptr►►►filedata_ptr►►►►stdout
**						refs == ?	refs == 1?
** ______________________________________________________________
** ______________________________________________________________
**
**							AFTER DUP2:
** 	  fd tables			fd struct	inode struct	file data
** parent_out	== 3►►►▼
** child_out  	== 3►►►►inode_ptr►►►filedata_ptr►►►►myfile
**					   ▲refs == 3	refs == 1
** child_stdout	== 1►►►▲
** """"""""""""""""""""""""""""""""""""""""""""""""""""""""""
**						inode_ptr►►►filedata_ptr►►►►stdout
**						refs == ?-1	refs == 1?
** ______________________________________________________________
** ______________________________________________________________
**
** Note that file tables are duplicated, but everything else is shared.
**
** Note that out starts with 2 references, the parent's and the child's, which
** is why the parent's reference also has to be closed. After the duplication of
** the fd pointed to by out, the myfile fd struct gains a third reference (the
** duplicated fd). I have no idea how many references stdin or stdout ultimately
** have... I guess it depends on the number of open processes? But it's not
** really relevant here. ;)
**
** The important thing is that once we hijack stdout/stdin from the fd table, it
** directs to our file instead of the stdout/stdin file, so anything that would
** normally go to or come from stdout/stdin will instead go to or come from our
** file.
**
** Note that to close myfile, we need to close ALL of its references, both in
** the parent and in the child. From what I have read, EOF is never returned in
** read unless all the unused fds are closed and all the used fds get data
** through them, so read will hang forever if there are unused fds pointing to
** a file. That is my rough understanding of why the superflous references must
** be eliminated.
**
** So, for the input fd and read(STDIN_FILENO, buf, size):
** ______________________________________________________________
** ______________________________________________________________
**
**							BEFORE CLOSING:
**
** 	  fd tables				fd struct	inode struct	file data
** ↓◄◄◄◄parent_in	== 3►►►▼
** ↓					   ▼
** ↓↓◄◄◄child_in  	== 3►►►►inode_ptr►►►filedata_ptr►►►►myfile
** ↓↓					   ▲refs == 3	refs == 1
** ↓↓↓◄◄child_stdin	== 0►►►▲
** ↓↓↳--------------->|
** ↓↳---------------->|<----WAITING_4_U♥
** ↳----------------->|			↑
** 						read(STDIN_FILENO, buf, size)
** ______________________________________________________________
** ______________________________________________________________
**
**						AFTER CLOSING EVERYTHING:
**
** 	  fd tables				fd struct	inode struct	file data
** x◄◄◄◄parent_in	== 3►x
**
**  x◄◄◄child_in  	== 3►x	inode_ptr►x	filedata_ptr►x	myfile
**   						refs == 0	refs == 0
**   x◄◄child_stdin	== 0►x
**					  |
**					  |<----4EVER_ALONE❧
**					  |			↑
** 						read(STDIN_FILENO, buf, size)
** ______________________________________________________________
** ______________________________________________________________
**
** Hence, in the parent we close all the parent's fds after we fork the child,
** and in the child we close all the inherited fds after we duplicate them to
** stdout/stdin, to end up like this before we execute the command:
** ______________________________________________________________
** ______________________________________________________________
**
**				AFTER CLOSING EVERYTHING EXCEPT STDIN:
**
** 	  fd tables				fd struct	inode struct	file data
** x◄◄◄◄close(parent_in)►x
**
**  x◄◄◄close(child_in►►►x ►inode_ptr►►►filedata_ptr►►►►myfile
**   					   ▲refs == 1	refs == 1
**   ↓◄◄child_stdin	== 0►►►▲
**   ↳--------------->|
**					  |<----WAITING_4_U♥
**					  |			↑
** 						read(STDIN_FILENO, buf, size)
** ______________________________________________________________
** ______________________________________________________________
**
** Thus, if we have a pipes.array, we close all the fds in the array, and if we
** have redir_out or redir_in fds, we close them too, once the ones we want are
** duplicated and assigned stdin/stdout. Remember, these fds should never be 0.
** And remember, kids, never leave your lover hanging, even in a threesome...
**
** Don't judge me for the while... xD
*/

void	child_process(char *exec_path, char *builtin, t_micli *micli)
{
	int		in;
	int		out;
	size_t	i;

	i = 0;
	in = 0;
	out = 0;
	get_new_stdin_stdout(&in, &out, micli);
	if (out)
		dup2(out, STDOUT_FILENO);
	if (in)
		dup2(in, STDIN_FILENO);
	if (micli->pipe_flag)
		while (i < micli->pipes.array_size)
			close(micli->pipes.array[i++]);
	if (micli->cmdline.fd_redir_in)
		close(in);
	if (micli->cmdline.fd_redir_out)
		close(out);
	child_process_exec(builtin, exec_path, micli);
}

/*
** This function will methodically check the exit status of a process and return
** the result. WIFEXITED returns true if the process terminated normally.
** WIFSIGNALED returns true if the process was terminated by a signal.
** WIFSTOPPED returns true if the process was stopped.
*/

int		get_child_exit_status(int stat_loc)
{
	int exit_status;

	exit_status = 0;
	if (WIFEXITED(stat_loc))
		exit_status = WEXITSTATUS(stat_loc);
	else if (WIFSIGNALED(stat_loc))
	{
		exit_status = WTERMSIG(stat_loc);
		if (WCOREDUMP(stat_loc))
			printf("HOSTIA UN COREDUMP O_O!!! RUN FOR YOUR LIVES!!\n");
	}
	else if (WIFSTOPPED(stat_loc))
		exit_status = WSTOPSIG(stat_loc);
	return (exit_status);
}

/*
** This function creates a child process, and then does a lot of cleaning up
** after it, so the function might more aptly be named child_process_aftermath.
**
** If a pipeline has been requested by the user, then the command will be
** launched in special pipeline mode and the parent will NOT wait for any child
** except the last child in the pipeline. The variable micli->pipes.array_size
** contains the number of file descriptors. The variable micli->pipes.count
** contains the number of pipes detected in the original line. The variable
** micli->pipes.cmd_index tracks the current command, starting with command 0.
**
** When pipes.count - pipes.cmd_index == 0, we have reached the last piped
** command. This can be graphically represented as follows:
**
** 					 -------->pipes.count == 3<--------
**					 ^				  ^				  ^
**					 ^  	pipe0	  ^		pipe1	  ^		pipe2
**					 |	[write][read] |	[write][read] | [write][read]
**			   stdin	1			0	3			2	5			4	stdout
**					↓	↑	   		↓	↑	  		↓	↑			↓	↑
**				  	cmd0			cmd1			cmd2			cmd3
**					child0			child1			child2			child3
**	cmd_index	==	0	----------> 1	---------->	2	---------->	3
**	pipes.count	==  3											  - 3
**																	↓
**																	0 == end cmd
**
** If we have reached the last command, all of the pipe file descriptors in the
** parent's process are closed and the parent waits for the last child to
** terminate. Unlike bash, only the last child's exit status is saved, but this
** technically isn't *pipe* functionality, but *exit status* functionality. :p
** If you REALLY want me to malloc an exit status array I'll do it, but I think
** it would kind of be wasting time. We know about it. ;p
**
** First, if the pipe_flag is in mode 1 or 3 (beginning or middle of a pipeline)
** we need to check for a pipeline that may have been broken by process
** failures. So we go do that with the broken_pipe_check. This will return a
** result we can later use to print errors as needed.
**
** Next, if any file descriptors were used to create or open files for redirect
** purposes, we close them.
**
** Next, we usually want to wait for our child to finish what it's doing.
** However, in a pipeline we only want to wait for our *last* child to finish,
** while we will not wait for any older children, because each child will wait
** for its own older sibling.
**
** If the pipe_flag is not set, this is an only child. If the pipe flag is set,
** then we are at the last child in the pipeline when the total number of pipes
** pipes minus the cmd_index equals 0. For example: echo test | cat | more has 2
** pipes, where echo is cmd_index 0, cat is cmd_index 1 and more is cmd_index 2.
** When cmd_index == 2, pipe_num - cmd_index is 2 - 2 == 0. So we know it's the
** last child that way.
**
** If we're waiting for the last child in a pipeline, the first thing we need to
** do is clear all the parent's references to the pipes by closing their fds,
** otherwise the children will hang around waiting for input from them that we
** don't intend to give them.
**
** Next, we change the function that executes upon a SIGINT signal to waiting.
** Miguel knows more about this, he did the signals. ;)
**
** Then we wait patiently for the child to finish.
**
** When it is finished we check its exit status and save the result.
**
** Then we reset the pipe_flag to 0.
**
** Lastly, sometimes exec_path will point directly to the memory occupied by
** cmd. This happens when the user has input an absolute or relative path to
** an executable. We don't want to free exec_path in that case, because cmd
** lives in the cmdline struct, which is freed elsewhere in the program, so it
** would cause a double-free.
*/

void	exec_child_process(char *exec_path, char *builtin, char *cmd, \
t_micli *micli)
{
	int		stat_loc;
	pid_t	pid;

	if (!(pid = fork()))
		child_process(exec_path, builtin, micli);
	if (micli->pipe_flag == 1 || micli->pipe_flag == 3)
		micli->cmd_result = broken_pipe_check(pid);
	if (micli->cmdline.fd_redir_out)
		close(micli->cmdline.fd_redir_out);
	if (micli->cmdline.fd_redir_in)
		close(micli->cmdline.fd_redir_in);
	if (!micli->pipe_flag || (micli->pipes.count - micli->pipes.cmd_index == 0))
	{
		clear_pipes(&micli->pipes, micli);
		signal(SIGINT, waiting);
		waitpid(pid, &stat_loc, WUNTRACED);
		micli->cmd_result = get_child_exit_status(stat_loc);
		micli->pipe_flag = 0;
	}
	if (exec_path != cmd)
		exec_path = ft_del(exec_path);
}
