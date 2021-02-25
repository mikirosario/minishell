#include "../minishell.h"

char	*ft_findvar(const char *name, size_t name_len, char **envp)
{	
	char	**ptr;

	ptr = NULL;
	if (envp && (ptr = envp))
		while (*ptr && (ft_strncmp(name, *ptr, name_len)))
			ptr++;
	return(*ptr);
}

int		find_pos(const char *name, char **envp, int countarr)
{
	int i;

	i = 0;
	while(envp[i] != name && i < countarr)
		i++;
	return (i);	
}

int		ft_countarr(char **envp)
{
	int i;

	i = 0;
	while(envp[i])
		i++;
	return(i);
}

char		**ft_envdup(char **envp)
{
	int		envp_size;
	int		i;
	char	**new_envp;

	envp_size = ft_countarr(envp);
	new_envp = malloc(sizeof(char *) * envp_size + 1);
	i = 0;
	while (i < envp_size)
	{
		new_envp[i] = ft_strdup(envp[i]);
		i++;
	}
	envp[i] = 0;
	return (new_envp);
}

void	increase(char **envp, t_micli *micli)
{
	*envp = ft_realloc(*envp, 5 * sizeof(char), micli);
}

// const char		*split_string(const char **argv)
// {
// 	int limit;
// 	int i;
// 	// char string;

// 	limit = '=';
// 	i = 0;
// 	// if (argv != NULL)
// 	// 	argv = string;
// 	while (argv[i] != '\0')
// 		if (argv[i] != limit)
// 	i++;
// 	return(argv[i]);
// }

// int		ft_check_key_argv(const char **argv)
// {
// 	int i;

// 	i = 0;
// 	while(argv[i] != '=' || argv[i] != '\0')
// 	{
// 		//check first letter is a letter or a '_'
// 		if (ft_isalpha(argv[1]) || argv[1] == '_')
// 			printf("first letter is ok \n");
// 		//check if the left string is valid
// 		if(ft_isalpha(argv[i + 1]) || ft_isdigit(argv[i + 1]) || argv[i] == '_')
// 			printf("second part is ok \n");
// 		i++;
// 	}
// 	//if its valid return 1
// 	return (1);
// }

int		ft_export(const char **argv, t_micli *micli)
{
	size_t name_len;
	const char *find;
	int i;
	int j;
	char *store;
	int countarr;
	char	*tmp_envp;
	int findpos;
	//char **micli->envp;
	// const char *checkargv;

	//if export has valid arguments arguments (ex: argument=) if it doesnt have a =
	//at the end of the argument it should return the prompt without any message or
	//setting any value in the env vars
	//dup envp in order to modify it
	//micli->envp = ft_envpdup(envp);
	countarr = ft_countarr(micli->envp);
	if (argv[1])
	{
		//check if values passed in argv are valid (letters, digits and the '_')
		//should report an error like "export: `#=a': not a valid identifier"
		//even if the character is escaped
		//also, the value inside the variable should be set inside double quot marks ("")
		//(without the user imput), also all existing variables content should be set inside
		//this quotation

		//check the length of argv
		name_len = ft_strlen(argv[1]);
		//check if argv is a existing variable
		find = ft_findvar(argv[1], name_len, micli->envp);
		if (find != 0) //found var
		{

			findpos = find_pos(argv[1], micli->envp, countarr);
			// if(findpos != 0)
			printf("%d\n", findpos);
			micli->envp[2] = "222";
			// ft_printf("%d\n", findpos);
			// ft_printf("%d\n", micli->envp[findpos]);
			// ft_printf("%p | %p\n", &find, ft_findvar2(argv[1], name_len, envp));
			ft_printf("var found\n");
		}
		// //else if
		if (find == 0) //not found
		{
			//micli->envp[25] = (char*)argv[2];
			printf("number of elements in array %d\n", countarr);
			// increase size of array
			//do things with data
			increase(micli->envp, micli);
			printf("number of elements in array %d\n", countarr);
			//
			free(micli->envp);
			//store = argv[1];
			ft_printf("var not found\n");
			//ft_memset(argv[1], )
			//create existing var
			//create_var = var_alloc((char*)argv[1], micli);
		}
	}
	//if export has no arguments
	if (!argv[1])
	{
		//order array in alphabetical reorder with bubble sort algo
		i = 0;
		while(i < countarr - 1)
		{
			j = 0;
			while (j < countarr - i - 1)
			{
				if(ft_strcmp(micli->envp[j], micli->envp[j + 1]) > 0)
				{
					tmp_envp = micli->envp[j];
					micli->envp[j] = micli->envp[j + 1];
					micli->envp[j + 1] = tmp_envp;
				}
				j++;
			}
			i++;
		}
		//this is ft_env
		store = *micli->envp;
		i = 0;
		while (store)
		{
			printf("declare -x %s\n", store);
			store = *(micli->envp + ++i);
		}
		return (0);
	}
	return (0);
}

