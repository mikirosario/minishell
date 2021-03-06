#include "../minishell.h"

int	ft_export(const char **argv, t_micli *micli)
{
	size_t	name_len;
	size_t	str_len;
	char	*find;

	if (argv[1])
	{
		name_len = ft_name_len(argv[1]);
		str_len = ft_strlen(argv[1]);
		find = find_var(argv[1], name_len, micli->envp);
		if (find != 0)
			upd(argv, str_len, name_len, micli);
		if (find == 0)
			new_var(argv, str_len, micli);
	}
	if (!argv[1])
		export_order(micli);
	export_print(micli);
	return (0);
}

void	upd(const char **argv, size_t str_len, size_t name_len, t_micli *micli)
{
	int		findpos;

	findpos = find_pos(argv[1], name_len, micli->envp);
	if (name_len != str_len)
	{
		free(micli->envp[findpos]);
		// micli->envp[findpos] = ft_comillas(argv[1], name_len, str_len);
		clean_calloc(str_len + 1, sizeof(char), micli);
		ft_memcpy(micli->envp[findpos], argv[1], str_len + 1);
	}
}

void	new_var(const char **argv, size_t str_len, t_micli *micli)
{
	int		i;
	char	**tmp;
	int		countarr;

	countarr = ft_countarr(micli->envp);
	i = 0;
	tmp = micli->envp;
	micli->envp = clean_calloc(countarr + 2, sizeof(char *), micli);
	while (i < countarr)
	{
		micli->envp[i] = tmp[i];
		i++;
	}
	free(tmp);
	micli->envp[countarr] = clean_calloc(str_len + 1, sizeof(char), micli);
	ft_memcpy(micli->envp[countarr], argv[1], str_len + 1);
}

int	export_print(t_micli *micli)
{
	int		i;
	char	*store;

	store = *micli->envp;
	i = 0;
	while (store)
	{
		ft_printf("declare -x %s\n", store);
		store = *(micli->envp + ++i);
	}
	return (0);
}
