#include "minishell.h"

/*
** This function executes built-ins when they are called from shell.
*/

int 	exec_builtin(char *cmd, t_micli *micli)
{
	if (!(ft_strcmp(cmd, "exit")))
		exit_success(micli);
	else if (!(ft_strcmp(cmd, "cd")))
		return(ft_cd((const char **)micli->cmdline->micli_argv, micli));
	else if (!(strcmp(cmd, "pwd")))
		return(ft_pwd(micli));
	else if (!(strcmp(cmd, "echo")))
		return(ft_echo((const char **)micli->cmdline->micli_argv, micli));
    else if (!(strcmp(cmd, "unset")))
        return(ft_unset(micli->cmdline->micli_argv, micli->envp));
    return(128);
}

// cd
// micli->envp
// **envp ->    *envp[0] key=value;
//              *envp[1] NULL;
// micli->cmdline->micli_argv;
// **argv ->    *argv[0] -> cmd
//              *argv[1] -> 1er argumento
//              *argv[2] -> NULL;
int     ft_cd(const char **argv, t_micli *micli) 
{
    char *home;

    //simple change directory if first argument exist
    if(argv[1] == 0 || *argv[1] == '~')
    {
        // 0 1 2 3 4 5
        // H O M E = /BLABLA
        //ft_printf("%s%s: ", home, (argv[1] + 2), strerror);
        home = find_var("HOME", 4, micli->envp);
        chdir(home += 5);
        //error habndling
        return(0);
    }

    if (argv[1] != 0 && *argv[1] != '~')
    {
        if (chdir(argv[1]) == -1)
        {
            ft_printf("cd: %s: %s\n", argv[1], strerror(errno));
            return(1);
        }
            return(0);
    }
    return(0);
    // here should update env pwd and old pwd, (where are you and where have you been)
    //
    // 
}

/*  
**  echo function checks first if it has arguments in case it doesnt it will print an empty line
**  then it checks if it has arguments equal to -n, if found sets the arglaf to 1 and increases the
**  counter, when all -n are read it starts writing all characters, when finished printing
**  checks if it the arglaf is 1 or 0, if is set to 0 it will print add a \n. 
*/

int    ft_echo(const char **argv, t_micli *micli)
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
        return(0);
    }

    i = 1;
    while(!(ft_memcmp(argv[i], n, len)))
    {
        micli->builtins.argflag = 1;
        if(argv[i+1] == NULL)
            return(0);
        i++;
    }
    while(argv[i] != 0)
    {
        ft_printf(argv[i + 1] ? "%s " : "%s", argv[i]);
            i++;
    }
    if(micli->builtins.argflag != 1)
            ft_printf("\n");
        return(0);
}

//pwd
int    ft_pwd()
{
    char cwd[FILENAME_MAX];
    
    if (getcwd(cwd, sizeof(cwd)) != NULL)
            ft_printf("%s\n", cwd);
        return(0);
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

int    ft_unset(char **argv, char **envp)
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
    return(0);
}

//env

// void    ft_env(const char **argv)
// {
    
// }