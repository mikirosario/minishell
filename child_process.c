/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/14 21:17:29 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/14 21:51:28 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function will execute a command as a builtin if it is a buitin, or a
** binary if it is a binary. If a builtin fails, we exit with exit_failure, and
** if it succeeds we exit with exit_success, to notify the parent of the result
** via waitpid. The execve function handles this internally.
*/

void	child_process_exec(char *builtin, char *exec_path, t_micli *micli)
{
	int	res;
	
	if (builtin != NULL)
	{
		res = exec_builtin(exec_path, micli); //function must return exit status of executed builtin
		if (!res)
		{
			freeme(micli);
			exit (EXIT_SUCCESS); //no quiero liberar mi memoria por cerrar el hijo
		}	
		else
		{
			freeme(micli);
			exit (EXIT_FAILURE);
		}
	}
	else
	{
		//freeme(micli); caca
		execve(exec_path, micli->cmdline.micli_argv, micli->envp);//now execute command... =_=
	}
}

void	child_process(char *exec_path, char *builtin, t_micli *micli)
{
	int 	in = 0; //for stdin replacement
	int 	out = 0; //for stdout replaceent
	int 	writefd_pos; //for stdout position in pipes array, if needed
	int 	readfd_pos; //for stdin position in pipes array, if needed
	size_t	i;

	i = 0;
	//readfd_pos = micli->pipes.cmd_index * 2; //Equation to derive appropriate read fd in the pipe array from the pipe number
	//writefd_pos = readfd_pos + 3; //Equation to derive apropriate write fd from the read fd	
	if (micli->pipe_flag) //if there is a pipe array, calculate the write and read fds for the pipe based on the pipe index.
	{
		writefd_pos = micli->pipes.cmd_index * 2 + 1; //New equation to derive appropriate write fd in the pipe array from the pipe number, with no need for initial phantom pipe
		readfd_pos = writefd_pos - 3; //Causes sign overflow for first pipe, but the first pipe in a set should always leave pipeflag in 01 state, so we should never use the overflow value... please give me a break on this one, compiler, I know what I'm doing... :p
	}
	if (micli->cmdline.fd_redir_out) //Redirects have precedence over pipes, which appears to be bash functionality, so: bc<in>out | cat will run 'bc' with 'in' as input, output result to 'out' and then run 'cat' with stdin as input, NOT with bc-out as input as in zsh
		out = micli->cmdline.fd_redir_out;
	else if (ft_isbitset(micli->pipe_flag, 0)) //if there is no output redirect but there is a pipe, use the pipe as output
		out = micli->pipes.array[writefd_pos]; 
	if (micli->cmdline.fd_redir_in)
		in = micli->cmdline.fd_redir_in;
	else if (ft_isbitset(micli->pipe_flag, 1))
		in = micli->pipes.array[readfd_pos];
	if (out)
	//close stdout (1), normally used for write to terminal, and make a duplicate
	// of fd[1], write end of pipe, and assign file descriptor 1 to it.
		dup2(out, STDOUT_FILENO);
	//close stdin (0) normally used for read from terminal, and make a duplicate
	//of fd[0], read end of pipe, and assign file descriptor 0 to it.
	if (in)
		dup2(in, STDIN_FILENO);
	//printf("PIPE FLAG: %u\nPIPE INDEX: %zu\nREAD FD: %d\nWRITE FD: %d\n", micli->pipe_flag, micli->pipes.cmd_index, readfd_pos, writefd_pos);
	
	// if (ft_isbitset(micli->pipe_flag, 0)) //if micli->pipe_flag rightmost bit is set: 11 == read-write or 01 == write-only
	// 	dup2(micli->pipes.array[writefd_pos], STDOUT_FILENO); //stdout == 1

	// if (ft_isbitset(micli->pipe_flag, 1)) //if micli->pipeflag leftmost bit is set: 11 == read-write or 10 == read-only
	// 	dup2(micli->pipes.array[readfd_pos], STDIN_FILENO); //stdin == 0;
	if (micli->pipe_flag)
		while (i < micli->pipes.array_size) //there are array_size fds (2 fds per pipe)
			close(micli->pipes.array[i++]);
	if (micli->cmdline.fd_redir_in)
		close(in);
	if (micli->cmdline.fd_redir_out)
		close(out);
	child_process_exec(builtin, exec_path, micli);
}

pid_t	exec_child_process(char *exec_path, char *builtin, char *cmd, t_micli *micli)
{
	int		stat_loc;
	pid_t	pid;
	size_t	i;
	
	i = 0;
	// if (builtin != NULL) //if builtin is defined, command is a builtin, después guarreo para evitar que al liberar exec_path se libere memoria apuntada por cmd antes de tiempo :p 
		// 	micli->cmd_result = exec_builtin(builtin, micli); //function must return exit status of executed builtin (piped builtins not yet functional)
		if (!(pid = fork())) //child inherits micli->pipes.array[] for pipes, micli->cmdline.fd_redir_out and micli->cmdline.fd_redir_in for redirects
			child_process(exec_path, builtin, micli);
		if (micli->cmdline.fd_redir_out)
			close(micli->cmdline.fd_redir_out);
		if (micli->cmdline.fd_redir_in)
			close(micli->cmdline.fd_redir_in);
		if (exec_path && (!micli->pipe_flag || (micli->pipes.count - micli->pipes.cmd_index == 0)))
		{	
			while (i < micli->pipes.array_size) //there are array_size fds (2 fds per pipe)
				close(micli->pipes.array[i++]);
			signal(SIGINT, waiting);
			waitpid(pid, &stat_loc, WUNTRACED);
			micli->cmd_result = WEXITSTATUS(stat_loc);
			if (micli->cmd_result == 127)
				ft_printf("micli: %s: %s\n", cmd, strerror(2));
			micli->pipe_flag = 0; //Reset pipe_flag
			//printf("CMD RESULT: %d\n", micli->cmd_result);
		}
		else if (micli->pipe_flag)
			micli->pipes.cmd_index++; //increment cmd_index for child pipe_count comparison
		if (exec_path != cmd) //guarreo para evitar liberar memoria apuntada por cmd antes de tiempo provocando un double free :p
			exec_path = ft_del(exec_path);
	return(pid);
}