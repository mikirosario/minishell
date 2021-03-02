#include "../minishell.h"

char	find_pos(const char *name, size_t name_len, char **envp)
{
	int i;

	i = 0;
	while (envp[i] != NULL && ft_strncmp(name, envp[i], name_len))
		i++;
	return (i);
}

// int		ft_countarr(char **envp)
// {
// 	int i;

// 	i = 0;
// 	while(envp[i])
// 		i++;
// 	return(i);
// }

// char		**ft_envdup(char **envp)
// {
// 	int		envp_size;
// 	int		i;
// 	char	**new_envp;

// 	envp_size = ft_countarr(envp);
// 	new_envp = malloc(sizeof(char *) * envp_size + 1);
// 	i = 0;
// 	while (i < envp_size)
// 	{
// 		new_envp[i] = ft_strdup(envp[i]);
// 		i++;
// 	}
// 	envp[i] = 0;
// 	return (new_envp);
// }

size_t	ft_name_len(const char *str)
{
	int	i;

	i = 0;
	while(str[i] != 0 && str[i] != '=')
		i++;
	return (i);
}

char	*ft_comillas(const char *str, size_t name_len, size_t str_len)
{
	char *ret;

	ret = malloc(str_len + 3);
	ft_memcpy(ret, str, name_len + 1);
	ret[name_len + 1] = '"';
	ft_memcpy(ret + name_len + 2, str + name_len + 1, str_len - name_len - 1);
	ret[str_len + 1] = '"';
	ret[str_len + 2] = '\0';

	return (ret);
}

int		ft_export(const char **argv, t_micli *micli)
{
	size_t	name_len;
	size_t	str_len;
	char	*find;
	int		i;
	int		j;
	char	*store;
	int		countarr;
	char	*tmp_envp;
	int		findpos;
	char	**tmp;

	//if export has valid arguments arguments (ex: argument=) if it doesnt have a =
	//at the end of the argument it should return the prompt without any message or
	//setting any value in the env vars
	//dup envp in order to modify it
	//micli->envp = ft_envpdup(envp);
	
	countarr = ft_countarr(micli->envp);
	printf("countarr %d\n", countarr);
	if (argv[1])
	{
		printf("arg is received \n");
		//check if values passed in argv are valid (letters, digits and the '_')
		//should report an error like "export: `#=a': not a valid identifier"
		//even if the character is escaped
		//also, the value inside the variable should be set inside double quot marks ("")
		//(without the user imput), also all existing variables content should be set inside
		//this quotation

		//check the length of argv
		name_len = ft_name_len(argv[1]);
		printf("name_len %zu\n", name_len);
		str_len = ft_strlen(argv[1]);
		printf("str_len %zu\n", str_len);
		//check if argv is a existing variable
		printf("finding var...\n");
		find = find_var(argv[1], name_len, micli->envp);
		printf("finding pos ...\n");
		findpos = find_pos(argv[1], name_len, micli->envp);
		printf("pos is %d\n", findpos);
		if (find != 0) //found var
		{
			printf("found var! \n");
			//check if argv is the same size as envp
			//if not free it, and resize it
			//then copy it
			if (name_len != str_len)
			{
				free(micli->envp[findpos]);
				micli->envp[findpos] = ft_comillas(argv[1], name_len, str_len);
				// clean_calloc(str_len + 1, sizeof(char), micli);
				// ft_memcpy(micli->envp[findpos], argv[1], str_len + 1);
			}
		}
		// //else if
		if (find == 0) //not found
		{
			printf("var not found! \n");
			i = 0;
			tmp = micli->envp;
			micli->envp = clean_calloc(countarr + 2, sizeof(char*), micli);
			while(i < countarr)
			{
				micli->envp[i] = tmp[i];
				i++;
			}
			free(tmp);
			micli->envp[countarr] = clean_calloc(str_len + 1, sizeof(char), micli);
			ft_memcpy(micli->envp[countarr], argv[1], str_len + 1);
		}
	}
	//if export has no arguments
	if (!argv[1])
	{
		printf("No arg, printing envp \n");
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

