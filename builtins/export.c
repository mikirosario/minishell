/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 21:18:00 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/11 19:49:07 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		if (find != 0 && var_check(argv[1]) == 1)
			upd(argv, str_len, name_len, micli);
		if (find == 0 && var_check(argv[1]) == 1)
			new_var(argv, str_len, micli);
	}
	if (!argv[1])
	{
		export_order(micli);
		export_print(micli);
	}
	return (0);
}

int		var_check(const char *str)
{
	int		len;
	int		i;

	i = 1;
	len = ft_strlen(str);
	while (ft_isalpha(str[0]) || str[0] == '_')
	{
		while (i < len)
		{
			if (!(isvarchar(str[i])))
			{
				ft_printf("🚀 export: '%s': not a valid identifier\n", str);
				return (0);
			}
			i++;	
		}
		return (1);
	}
	if (!(ft_isalpha(str[0] || str[0] != '_')))
		ft_printf("🚀 export: '%s': not a valid identifier\n", str);
		return (0);
}

void	upd(const char **argv, size_t str_len, size_t name_len, t_micli *micli)
{
	int		findpos;

	findpos = find_pos(argv[1], name_len, micli->envp);
	if (name_len != str_len)
	{
		free(micli->envp[findpos]);
		micli->envp[findpos] = clean_calloc(str_len + 1, sizeof(char), micli);
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
	int		name_len;
	int		str_len;

	store = *micli->envp;
	i = 0;
	while (store)
	{
		name_len = ft_name_len(store);
		str_len = ft_strlen(store);
		ft_printf("declare -x ");
		write(1, store, name_len + 1);
		if (str_len != name_len)
		{
			ft_putchar('\"');
			ft_printf("%s\"\n", store + name_len + 1);
		}
		else
			ft_putchar('\n');
		store = *(micli->envp + ++i);
	}
	return (0);
}

// int	export_print(t_micli *micli)
// {
// 	int		i;
// 	int		j;
// 	char	**store;
// 	int		name_len;
// 	char	countarr;
// 	int		str_len;
// 	char	*tmp_envp;

// 	store = micli->envp;
// 	countarr = ft_countarr(micli->envp);
// 	i = 0;
// 	while(i < countarr - 1)
// 	{
// 		j = 0;
// 		while (j < countarr - i - 1)
// 		{
// 			if (ft_strcmp(store[j], store[j + 1]) > 0)
// 			{
// 				tmp_envp = store[j];
// 				store[j] = store[j + 1];
// 				store[j + 1] = tmp_envp;
// 			}
// 			j++;
// 		}
// 		i++;
// 	}
// 	i = 0;
// 	while (*store)
// 	{
// 		name_len = ft_name_len(*store);
// 		str_len = ft_strlen(*store);
// 		ft_printf("declare -x ");
// 		write(1, *store, name_len + 1);
// 		if (str_len != name_len)
// 		{
// 			ft_putchar('\"');
// 			ft_printf("%s\"\n", *store + name_len + 1);
// 		}
// 		else
// 			ft_putchar('\n');
// 		*store = *(store + ++i);
// 	}
// 	return (0);
// }
