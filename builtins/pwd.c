/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 17:11:55 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/19 20:45:25 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_pwd(const char **argv)
{
	char	cwd[FILENAME_MAX];

	if (argv[1] == 0 || *argv[1] != '-')
	{
		if (getcwd(cwd, sizeof(cwd)) != NULL)
			printf("%s\n", cwd);
		return (0);
	}
	if (*argv[1] == '-')
		printf("Argument \"\%s\" is not supported in micli\n", argv[1]);
	return (1);
}
