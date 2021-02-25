#include "../minishell.h"

/*
** This function executes built-ins when they are called from shell.
*/

int		exec_builtin(char *cmd, t_micli *micli)
{
	if (!(ft_strcmp(cmd, "exit")))
		exit_success(micli);
	else if (!(ft_strcmp(cmd, "cd")))
		return (ft_cd((const char **)micli->cmdline->micli_argv, micli));
	else if (!(ft_strcmp(cmd, "pwd")))
		return (ft_pwd((const char**)micli->cmdline->micli_argv));
	else if (!(ft_strcmp(cmd, "echo")))
		return (ft_echo((const char **)micli->cmdline->micli_argv, micli));
	else if (!(ft_strcmp(cmd, "unset")))
		return (ft_unset(micli->cmdline->micli_argv, micli->envp, micli));
	else if (!(ft_strcmp(cmd, "env")))
		return (ft_env(micli->envp));
	else if (!(ft_strcmp(cmd, "export")))
		return (ft_export((const char **)micli->cmdline->micli_argv, micli));
	return (128);
}

// cd
// micli->envp
// **envp ->    *envp[0] key=value;
//              *envp[1] NULL;
// micli->cmdline->micli_argv;
// **argv ->    *argv[0] -> cmd
//              *argv[1] -> 1er argumento
//              *argv[2] -> NULL;
int		ft_cd(const char **argv, t_micli *micli)
{
	char *home;

	//simple change directory if first argument exist
	if (argv[1] == 0 || *argv[1] == '~')
	{
		// 0 1 2 3 4 5
		// H O M E = /BLABLA
		//ft_printf("%s%s: ", home, (argv[1] + 2), strerror);
		home = find_var("HOME", 4, micli->envp);
		chdir(home += 5);
		//error habndling
		return (0);
	}

	if (argv[1] != 0 && *argv[1] != '~')
	{
		if (chdir(argv[1]) == -1)
		{
			ft_printf("cd: %s: %s\n", argv[1], strerror(errno));
			return (1);
		}
			return (0);
	}
	return (0);
	// here should update env pwd and old pwd, (where are you and where have you been)
	// * old pwd should only be set after one cd move
	// 
}

/*  
**  echo function checks first if it has arguments in case it doesnt it will print an empty line
**  then it checks if it has arguments equal to -n, if found sets the argflag to 1 and increases the
**  counter, when all -n are read it starts writing all characters separated with spaces till the last
**  one where it dosent print the last space.
**  When finished printing checks if it the argflag is 1 or 0, if is set to 0 it will print add a \n. 
*/

int		ft_echo(const char **argv, t_micli *micli)
{
	int i;
	char *n;
	int len;
   
	n = "-n";
	len = 2;
	i = 1;
	micli->builtins.argflag = 0;

	if (!argv[i])
	{
		ft_printf("\n");
		return (0);
	}

	i = 1;
	while (!(ft_memcmp(argv[i], n, len)))
	{
		micli->builtins.argflag = 1;
		if (argv[i + 1] == NULL)
			return (0);
		i++;
	}
	while(argv[i] != 0)
	{
		ft_printf(argv[i + 1] ? "%s " : "%s", argv[i]);
		i++;
	}
	if (micli->builtins.argflag != 1)
			ft_printf("\n");
		return (0);
}

int		ft_pwd(const char **argv)
{
	int i;
	char cwd[FILENAME_MAX];
	
	i = 1;
	if (argv[i] == 0 || *argv[1] != '-')
	{
	   if (getcwd(cwd, sizeof(cwd)) != NULL)
			ft_printf("%s\n", cwd);
		return (0); 
	}
	if (*argv[i] == '-')
		ft_printf("Argument \"\%s\" is not supported in micli\n", argv[i++]);
	return (0);
}

// int		ft_putenv(const char *string)
// {
// 	const char *name_end;
// 	char *name;

// 	name_end = ft_strchr(string, '=');
// 	if (name_end)
// 	{
// 		name = (char *)malloc(name_end - string + 1);
// 		ft_memcpy(name, string, name_end - string);
// 		name[name_end - string] = '\0';
// 		return (setenv (name, name_end + 1, 1));
// 	}
// 	ft_unset (string, );
// 	return (0);
// }
/****END EXPORT***/

/*
**  deletes variables and enviorement funcionts
**  if argv is not null, name_len equals to the legth of argv
**  then uses find_var function, if found delete it.
*/

// int		ft_unset(char **argv, char **envp)
// {
// 	size_t name_len;
// 	char *store;

// 	if (argv[1] != 0)
// 	{
// 		name_len = ft_strlen(argv[1]);
// 		store = find_var(argv[1], name_len, &envp[1]);
// 		if ( store != 0)
// 			ft_memset(store, 0, sizeof(store));
// 	}
// 	return (0);
// }


int		ft_unset(char **argv, char **envp, t_micli *micli)
{
	size_t name_len;
	char *store;
	int i = 0;
	char **temp;
	// int size = 0;
	// int pos = 0;
	// size_t size;
	while(envp[i])
		i++;
	printf("elements in array %d\n", i);

	if (argv[1] != 0)
	{
		name_len = ft_strlen(argv[1]);
		store = find_var(argv[1], name_len, &envp[1]);
		if ( store != 0)
		{
			//set element to 0
			ft_memset(store, 0, sizeof(store));
			//reorder array and remove empty element
			temp = envp;
			micli->envp = clean_calloc(i, sizeof(char *), micli);
			i = 0;
			while(*temp)
			{
				if(*temp != store)
					micli->envp[i++] = *temp;
				temp++;
			}
			// free(temp);
		}
	}
	return (0);
}

/*
**  Function that prints the enviorenment variables stored in envp
*/

int		ft_env(char **envp)
{
	int i;
	char *store;

	store = *envp;
	i = 0;
	while (store)
	{
		printf("%s\n", store);
		store = *(envp + ++i);
	}
	return (0);
}
