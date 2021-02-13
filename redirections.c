#include "minishell.h"

int     ft_redir()
{
    int fd0;
    int fd1;
    int i;
    int in;
    int out;
    char input[64];
    char output[64];

    in = 0;
    out = 0;

    if(pid == 0)

    /*
    **  finds where '<' or '>' occurs and make that argv[i] = NULL
    **  to ensure that command wont't read that
    */

    i = 0;
    while(argv[i] 1= '\0' && ++i)
    {
        if(ft_strcmp(argv[i], "<") == 0)
        {
            argv[i] = NULL;
            ft_strcpy(input, argv[i + 1]);
            in = 2;
        }

        if(ft_strcmp(argv[i], ">") == 0)
        {
            argv[i] = NULL;
            ft_strcpy(output, argv[i + 1])
            out = 2;
        }
    }
    /*
    ** if '<' char was found on input
    */
   if(in)
   {
       if((fd0 = open(input, O_RDONLY, 0)) < 0)
       {
           perror("Can't open input file");
           exit(0);
       }
   }
}