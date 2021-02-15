#include "minishell.h"

// int   catch_signal() 
// {
//    int status;
//    pid_t result;

//    result = waitpid(pid, &status, WNOHANG);
//    if (result == 0)
//    {
//       // Child still alive
//       signal(SIGINT, ctrl_c);
//       signal(SIGQUIT, ctrl_bar);
//    }
//    else if (result == -1)
//    {
//       // error
//    }
//    else
//    {
//       // child exited
//       // if ctrl_c return prompt
//       // if ctrl_bar return nothing
//    }
//    //signal(SIGCHLD, SIG_IGN);
   
//    return(0);
// }

// int   catch_signal() 
// {
//    signal(SIGINT, ctrl_c);
//    signal(SIGQUIT, ctrl_bar);
//    return(0);
// }
/*
** Ctrl+C interrupt the program and terminates the application
*/

void	ctrl_c(int signum)
{
   //if child process is running do...
   //if(micli->child_know.child_flag == 1)
      printf("Ctrl -%d\n", signum);
   //else return a new prompt
   //else if(ft_printf("else ctrl_c"));
}

/*
** Ctrl+\ tells the application to exit as soon as possible without saving
*/

void	ctrl_bar(int signum)
{
   //if child process is runnning do..
   //if(micli->child_know.child_flag == 1)
	   printf("Ctrl -%dQuit: 3\n", signum);
   //else do nothing
   //else if(ft_printf("else ctrl_c"));
}

/*
** Ctrl+D ends input stream
*/

// void  ctrl_d(int signum)
// {

// }
