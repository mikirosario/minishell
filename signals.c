#include "minishell.h"

// typedef void (*sighandler_t)(int);

// sighandler_t signal(int signum, sighandler_t handler);
//
//first arg is the signal number, each signal has a predefined integer value assigned.
//ex: 2 = SIGINT and 20 SIGTSTP
//second arg is a pointer to a singal handler. The signal handler is a function that
//must accept an int as the only parameter and return void as descibed by the typedef

// void sigint_handler()
// {
//     printf("Caught crtl-c\n");
// 	signal(SIGINT, SIG_IGN);
// }

// //ctrl+C interrupt the program and terminates the application
void	signal_c()
{
	(void) signal(SIGINT, SIG_DFL);
}
// //ctrl+\
// tells the application to exit as soon as possible without saving anything;
void	singal_barra()
{
	(void) signal(SIGQUIT, SIG_DFL);
}
// ctrl+D generates a EOF/end of imput (normally EOF=-1)
void	signal_d()
{
	//send EOF (?)
}