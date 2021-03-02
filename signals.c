/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/28 19:30:53 by mvillaes          #+#    #+#             */
/*   Updated: 2021/02/28 20:07:44 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	catch_signal() 
{
	// int status;

	// status = ft_child_status();
	// if (status == 0)// Child still alive
	// {
		signal(SIGINT, ctrl_c);
		signal(SIGQUIT, ctrl_bar);
		signal(4, ctrl_d);
	// }
	// if (status == -1)
	// 	ft_printf("Signal error");
}

// int		ft_child_status()
// {
// 	int status;
// 	pid_t child_status;
// 	pid_t pid;

// 	pid = fork();
// 	child_status = waitpid(pid, &status, WNOHANG);
// 	if (child_status == 0) //child alive
// 		return (0);
// 	if (child_status == -1) //error
// 		return (-1);
// 	return (1);
// }
// void	ft_wait(int signum)
// {
// 	wait(NULL);
// }

/*
** Ctrl+C interrupt the program and terminates the application
*/

void	ctrl_c()
{
   //if child process is running do...
   //if(micli->child_know.child_flag == 1)
	ft_printf("\n");
   //else return a new prompt
   //else if(ft_printf("else ctrl_c"));
}

/*
** Ctrl+\ tells the application to exit as soon as possible without saving
*/

void	ctrl_bar()
{
   //if child process is runnning do..
   //if(micli->child_know.child_flag == 1)
	ft_printf("Quit: 3\n");
   //else do nothing
   //else if(ft_printf("else ctrl_c"));
}

void	ctrl_d()
{
	signal(SIGCHLD, SIG_IGN);
	// Parent process installs signal handler for the SIGCHLD
	// calls wait, when child terminated the SIGCHLD is delivered
	// signal(SIGCHLD, ft_wait(signum));
	// ctrl_d;
	write(1, "exit\n", 5);
	exit(0);
}
