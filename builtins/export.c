/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 21:18:00 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/22 19:15:14 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int		ft_export(const char **argv, t_micli *micli)
{
	int		z;
	int		res;

	z = 1;
	while (argv[z])
	{
		res = var_op(argv, micli, z);
		z++;
	}
	if (!argv[1])
		export_print(micli);
	return (res);
}

int		var_check(const char *str)
{
	int		len;
	int		i;

	i = 1;
	len = ft_strlen(str);
	while (ft_isalpha(str[0]) || str[0] == '_')
	{
		while (i < len && str[i] != '=')
		{
			if (!(isvarchar(str[i])))
			{
				ft_printf("💥 export: '%s': not a valid identifier\n", str);
				return (0);
			}
			i++;
		}
		return (1);
	}
	if (!(ft_isalpha(str[0] || str[0] != '_')))
		ft_printf("💥 export: '%s': not a valid identifier\n", str);
	return (0);
}

void	upd(const char **argv, size_t name_len, t_micli *micli, int z)
{
	int		findpos;
	size_t	str_len;

	str_len = ft_strlen(argv[z]);
	findpos = find_pos(argv[z], name_len, micli->envp);
	free(micli->envp[findpos]);
	micli->envp[findpos] = clean_calloc(str_len + 1, sizeof(char), micli);
	ft_memcpy(micli->envp[findpos], argv[z], str_len + 1);
}

void	new_var(const char **argv, size_t str_len, t_micli *micli, int z)
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
	ft_memcpy(micli->envp[countarr], argv[z], str_len + 1);
}

/*
** Export print now working with a mask created in export_order that provides
** a list of numbers corresponding to envp in alphabetical order. -Miki
*/

void	export_print(t_micli *micli)
{
	size_t	*mask;
	size_t	i;
	size_t	name_len;
	size_t	str_len;
	size_t	countarr;

	mask = export_order(micli);
	countarr = ft_countarr(micli->envp);
	i = 0;
	while (i < countarr)
	{
		name_len = ft_name_len(micli->envp[mask[i]]);
		str_len = ft_strlen(micli->envp[mask[i]]);
		ft_printf("declare -x ");
		write(1, micli->envp[mask[i]], name_len + 1);
		if (str_len != name_len)
		{
			ft_putchar('\"');
			ft_printf("%s\"\n", micli->envp[mask[i]] + name_len + 1);
		}
		else
			ft_putchar('\n');
		i++;
	}
	mask = ft_del(mask);
}
