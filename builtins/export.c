/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 21:18:00 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/10 19:12:28 by mvillaes         ###   ########.fr       */
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

	i = 0;
	len = ft_strlen(str);
	while (ft_isalpha(str[i]) || str[i] == '_')
	{
		while (i < len)
		{
			while (isvarchar(str[i]) == 0)
				return (0);
			while (isvarchar(str[i]) == 1)
			{
				return (1);
				i++;
			}	
		}
	}
	if (!(ft_isalpha(str[i] || str[i] != '_')))
		ft_printf("ERROR: invalid character on export function\n");
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