#include "../minishell.h"



// void	ft_change(char **array, int length)
// {
// 	int i;
	
// 	*array = malloc(length * sizeof(int));
// 	i = 0;
// 	while(i < length)
// 	{
// 		(*array)[i] = 1;
// 		i++;
// 	}
// }

char	*ft_findvar(const char *name, size_t name_len, char **envp)
{	
	char	**ptr;

	ptr = NULL;
	if (envp && (ptr = envp))
		while (*ptr && (ft_strncmp(name, *ptr, name_len)))
			ptr++;
	return(*ptr);
}

int		ft_countarr(char **envp)
{
	int i;

	i = 0;
	while(envp[i])
		i++;
	return(i);
}


int		ft_export(const char **argv, char **envp, t_micli *micli)
{
	size_t name_len;
	const char *find;
	int i;
	char *store;
	int countarr;
	int pos;

	//if export has valid arguments arguments (ex: argument=) if it doesnt have a =
	//at the end of the argument it should return the prompt without any message or
	//setting any value in the env vars
	countarr = ft_countarr(envp);
	if (argv[1])
	{
		name_len = ft_strlen(argv[1]);
		find = ft_findvar(argv[1], name_len, micli->envp);
		if (find != 0) //found var
		{
			ft_printf("var found\n");
			//set existing var to 0;
			// if (argv[1] != 0)
			// {
			// 	name_len = ft_strlen(argv[1]);
			// 	store = find_var(argv[1], name_len, &envp[1]);
			// 	if ( store != 0)
			// 		ft_memset(store, 0, sizeof(store));
			// }
			// return (0);
			//update existing var
			ft_realloc((char *)&find, name_len, micli);
			//argv[1] = &var_store;

			//mod_var = ft_realloc((char*)argv[1], name_len, micli);
			//if (mod_var == NULL)
			// 	ft_printf("realloc fail\n");

		}
		// //else if
		if (find == 0) //not found
		{
			pos = countarr++;
			
			printf("pos where var will be insterted is %d\n", pos);
			printf("pos 26 content %s\n", envp[26]);
			printf("pos 27 content %s\n", envp[27]);
			printf("pos 28 content %s\n", envp[28]);
			envp[pos] = argv[1];
			printf("pos 26 content %s\n", envp[26]);
			printf("pos 27 content %s\n", envp[27]);
			printf("pos 28 content %s\n", envp[28]);
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
		//!!!!missing alphabetical reorder
		//this is ft_env
		store = *envp;
		i = 0;
		while (store)
		{
			printf("declare -x %s\n", store);
			store = *(envp + ++i);
		}
		return (0);
	}
	return (0);
}

