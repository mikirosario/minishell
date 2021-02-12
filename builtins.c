#include "minishell.h"

/*
** This function executes built-ins when they are called from shell.
*/

void	exec_builtin(char *cmd, t_micli *micli)
{
	if (!(ft_strcmp(cmd, "exit")))
		exit_success(micli);
	if (!(ft_strcmp(cmd, "cd")))
		ft_cd((const char **)micli->cmdline->micli_argv, micli->envp, micli);
	if (!(strcmp(cmd, "pwd")))
		ft_pwd(micli);
	if (!(strcmp(cmd, "echo")))
		ft_echo((const char **)micli->cmdline->micli_argv, micli);
}

// cd
// micli->envp
// **envp ->    *envp[0] key=value;
//              *envp[1] NULL;
// micli->cmdline->micli_argv;
// **argv ->    *argv[0] -> cmd
//              *argv[1] -> 1er argumento
//              *argv[2] -> NULL;
void     ft_cd(const char **argv, char **envp, t_micli *micli) 
{
    char *get_dir;
    char *dir;
    char *where;

    
    
    
    // int i;

    // (void)micli;
    // //if is one arg
    // i = -1;
    // //if only cd is received, move to home directory
    // if (chdir(argv[1]) != 0)
    // {
    //     while(envp[++i])
    //         ft_strncmp("HOME", envp[i], 4);
    //     chdir((const char *)envp[i + 1]);
    // }
    // return(0);
    // if (chdir(argv[1] != 1))
    // {

    // }
    //     // strerror("cd: not such file or directory:" input)
    // //
    // while (envp[i])
    //     ft_strncmp("PWD", envp[i], 3);

    //     ft_realloc()
}

/*  
**  echo function checks first if it has arguments in case it doesnt it will print an empty line
**  then it checks if it has arguments equal to -n, if found sets the arglaf to 1 and increases the
**  counter, when all -n are read it starts writing all characters, when finished printing
**  checks if it the arglaf is 1 or 0, if is set to 0 it will print add a \n. 
*/

void    ft_echo(const char **argv, t_micli *micli)
{
    int i;
    char *n;
    int len;

    n = "-n";
    len = 2;
    i = 1;
    micli->builtins.argflag = 0;

    if(!argv[i])
    {
        ft_printf("\n");
        return;
    }

    i = 1;
    while(!(ft_memcmp(argv[i], n, len)))
    {
        micli->builtins.argflag = 1;
        if(argv[i+1] == NULL)
            return;
        i++;
    }
    while(argv[i] != 0)
    {
        ft_printf("%s ", argv[i]);
            i++; 
    }
    if(micli->builtins.argflag != 1)
            ft_printf("\n");
}

//pwd
void    ft_pwd()
{
    char cwd[FILENAME_MAX];
    
    if (getcwd(cwd, sizeof(cwd)) != NULL)
            ft_printf("%s\n", cwd);
    // pwd in bash when passed an argument with a -1234
    // returns:
    // bash: pwd: -1: invalid option
    // pwd: usage: pwd [-LP]
    // where -1 is the first char after -
}
//export

/*
**  unset
**  deletes variables and enviorement funcionts
**  if argv is not null, argv_length equals to the legth of argv
**  then uses find_var function, if found delete it. 
*/

void    ft_unset(char **argv, char **envp)
{
    int i;
    int argv_length;

    i = 1;
    //argv_length = 0;
    if(argv[i] != NULL)
    {
        argv_length = ft_strlen(argv[i]);

        if(find_var(argv[i], argv_length, &envp[i]))
        {
            envp[i] = NULL;
        }
    }
}

//env

// void    ft_env(const char **argv)
// {
    
// }