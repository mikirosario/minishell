#include "minishell.h"

int     ft_redir(const char **argv, t_micli *micli)
{
    int fd0;
    int fd1;
    int i;
    int in;
    int out;
    char input[64];
    char output[64];
    pid_t pid;

    /*
    **  finds where '<' or '>' occurs and make that argv[i] = NULL
    **  to ensure that command wont't read that
    */

    if(pid == 0)
    {
        in = 0;
        out = 0;
        while(argv[i] != '\0')
        {
            i = 0;
            if(ft_strcmp(argv[i], "<") == 0)
            {
                argv[i] = NULL;
                ft_strcpy(input, argv[i + 1]);
                in = 2;
            }

            if(ft_strcmp(argv[i], ">") == 0)
            {
                argv[i] = NULL;
                ft_strcpy(output, argv[i + 1]);
                out = 2;
            }
            i++;
        }
        /*
        ** if '<' char was found on input
        */
        if(in)
        {
            ft_alef(in, micli);
            // //fd0 is file descriptor
            // if((fd0 = open(input, O_RDONLY, 0)) < 0)
            // {
            //     print_error("Can't open input file", "redirection <");
            //     return(0);
            //     //exit(0);
            // }
            // //dup2() copies content of fdo in input of preceeding file
            // dup2(fd0, 0);
            // close(fd0);
        }
        /* 
        **  if '>' char was found on input
        */
       if(out)
       {
           ft_aright(out, micli);
        //    int fd1;
        //    if((fd1 = /*create???*/(output, /*0644???*/)) < 0)
        //    {
        //        print_error("Can't open output file", "redirection >");
        //        return(0);
        //        //exit(0);
        //    }
        //    dup2(fd1, STDOUT_FILENO);
        //    close(fd1);
       }
       if (!(execvp(*argv, argv) >= 0))
       {
           ft_printf("ERROR: exec failed\n");
           return(1);
           //exit(1);
       }
    }
    else if((pid) < 0)
    {
        printf("fork() failed\n");
        return(1);
        //exit(1);
    }
    else
        while(!(wait(/*&status???*/) == pid));
}



////CLEAN
int     ft_aleft(const char **argv, t_micli *micli)
{
    int     fd0;
    char    input[64];
    //fd0 is file descriptor
    if((fd0 = open(input, O_RDONLY, 0)) < 0)
    {
        print_error("Can't open input file", "redirection <");
        return(0);
        //exit(0);
    }
    //dup2() copies content of fdo in input of preceeding file
    dup2(fd0, 0);
    close(fd0);
}

int     ft_aright(const char **argv, t_micli *micli)
{
    int fd1;
    if((fd1 = /*create???*/(output, /*0644???*/)) < 0)
    {
        print_error("Can't open output file", "redirection >");
        return(0);
        //exit(0);
    }
    dup2(fd1, STDOUT_FILENO);
    close(fd1);
}