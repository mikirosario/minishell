#include "minishell.h"

// cd
// micli->envp
// **envp ->    *envp[0] key=value;
//              *envp[1] NULL;
// micli->cmdline->micli_argv;
// **argv ->    *argv[0] -> cmd
//              *argv[1] -> 1er argumento
//              *argv[2] -> NULL;
int     ft_cd(const char **argv, char **envp, t_micli *micli) 
{
    int i;

    (void)micli;
    //if is one arg
    i = -1;
    //if only cd is received, move to home directory
    if (chdir(argv[1]) != 0)
    {
        while(envp[++i])
            ft_strncmp("HOME", envp[i], 4);
        chdir((const char *)envp[i + 1]);
    }
    return(0);
    // if (chdir(argv[1] != 1))
    // {

    // }
    //     // strerror("cd: not such file or directory:" input)
    // //
    // while (envp[i])
    //     ft_strncmp("PWD", envp[i], 3);

    //     ft_realloc()
}

//echo
void    ft_echo(const char **argv, t_micli *micli)
{
    int i;
    char *n;
    int len;

    n = "-n";
    len = 2;
    i = 1;
    micli->builtins.argflag = 0;

    //check if echo has arguments
    //if not, print a empty line

    if(argv[i] == NULL)
    {
        ft_printf("\n");
        return;
    }
    
    //check if arguments are -n
    //if found set argflag to 1
    //and increase the counter

    i = 1;
    while(!(ft_memcmp(argv[i], n, len)))
    {
        micli->builtins.argflag = 1;
        if(argv[i+1] == NULL)
            return;
        i++;
    }

    //after arguments are checked
    //check if there is more text
    //and print it
    

    while(argv[i] != 0)
    {
        ft_printf("%s ", argv[i]);
            i++; 
    }

    //if no -n print a return
    if(micli->builtins.argflag != 1)
            ft_printf("\n");
}

//pwd
void    ft_pwd(t_micli *micli)
{
    char cwd[FILENAME_MAX];
    
    if(micli->cmdline->arguments == 0)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
                ft_printf("%s\n", cwd);
    }
    if (micli->cmdline->arguments > 0)
        ft_printf("pwd: too many arguments \n");
}
//export

//unset

//env