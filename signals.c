#include "minishell.h"

//typedef void (*sighandler_t)(int);

//sighandler_t signal(int signum, sighandler_t handler);
//
//first arg is the signal number, each signal has a predefined integer value assigned.
//ex: 2 = SIGINT and 20 SIGTSTP
//second arg is a pointer to a singal handler. The signal handler is a function that
//must accept an int as the only parameter and return void as descibed by the typedef


int catch_signal() 
{
   signal(SIGINT, ctrl_c);
   signal(SIGQUIT, ctrl_bar);

//    while(1) {
//       //printf("Going to sleep for a second...\n");
//       sleep(1); 
//    }
   return(0);
}

void	ctrl_c(int signum)
{
   printf("Ctrl -%d\n", signum);
   //exit(1);
}

void	ctrl_bar(int signum)
{
	printf("Ctrl -%d\n", signum);
}

// void	ctrl_d(int signum)
// {
// 	printf("Ctrl -%d\n", signum);
// }

// //ctrl+C interrupt the program and terminates the application
// void	signal_c()
// {
// 	// if program is executing
// 	(void) signal(SIGINT, SIG_DFL);
// 	// if no program is executing
// 	// new line
// }
// // //ctrl+bar
// // tells the application to exit as soon as possible without saving anything;
// void	singal_bar()
// {
// 	// 
// 	(void) signal(SIGQUIT, SIG_DFL);
// }
// // ctrl+D generates a EOF/end of imput (normally EOF=-1)
// void	signal_d()
// {
// 	//send EOF (?)
// }


// usefull linke
// http://people.cs.pitt.edu/~alanjawi/cs449/code/shell/UnixSignals.htm