/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_op.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/22 18:59:58 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/22 19:18:07 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	var_op(const char **argv, t_micli *micli, int z)
{
	size_t	name_len;
	size_t	str_len;
	char	*find;

	name_len = ft_name_len(argv[z]);
	str_len = ft_strlen(argv[z]);
	find = find_var(argv[z], name_len, micli->envp);
	if (ft_strncmp(argv[z], "_=", 2) && ft_strncmp(argv[z], "_\0", 2))
	{
		if (find != NULL)
		{
			if (ft_strchr(argv[z], '=') != NULL)
				upd(argv, name_len, micli, z);
		}
		else if (var_check(argv[z]) == 1)
			new_var(argv, str_len, micli, z);
		else
			return (1);
	}
	return (0);
}
