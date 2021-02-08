#include "minishell.h"

// cd
// micli->envp
// **envp ->    *envp[0] key=value;
//              *envp[1] NULL;
// micli->token->micli_argv;
// **argv ->    *argv[0] -> cmd
//              *argv[1] -> 1er argumento
//              *argv[2] -> NULL;
int     cd(const char **argv, char **envp, t_micli *micli) 
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

//pwd
void    pwd(t_micli *micli)
{
    char cwd[FILENAME_MAX];
    
    if(micli->token->arguments == 0)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
                ft_printf("%s\n", cwd);
    }
    if (micli->token->arguments > 0)
        ft_printf("pwd: too many arguments \n");
}
//export

//unset

//env
