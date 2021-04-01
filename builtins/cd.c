/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 17:08:14 by mvillaes          #+#    #+#             */
/*   Updated: 2021/04/01 21:54:21 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_cd(const char **argv, t_micli *micli)
{
	char	*home;

	oldpwd(micli);
	if (argv[1] == NULL || \
	(*argv[1] == '~' && (argv[1][1] == '/' || argv[1][1] == '\0')))
	{
		home = find_var("HOME", 4, micli->envp);
		if (!home)
			return (1);
		if (argv[1] && ft_strlen(argv[1]) > 2)
		{
			home = clean_ft_strjoin((home + 5), &argv[1][1], micli);
			chdir(home);
			home = ft_del(home);
		}
		else
			chdir(home += 5);
		update_pwd(micli);
		return (0);
	}
	else
		if (cd_helper(argv, micli))
			return (1);
	return (0);
}

int	cd_helper(const char **argv, t_micli *micli)
{
	if (chdir(argv[1]) == -1)
	{
		printf("cd: %s: %s\n", argv[1], strerror(errno));
		return (1);
	}
	update_pwd(micli);
	return (0);
}

void	oldpwd(t_micli *micli)
{
	const char	*oldpwd;
	char		path[PATH_MAX];
	const char	*current;
	char		*tmp;
	const char	**fake_argv;

	current = getcwd(path, sizeof(path));
	oldpwd = "OLDPWD=";
	tmp = ft_strjoin(oldpwd, current);
	fake_argv = clean_calloc(3, sizeof(char *), micli);
	fake_argv[1] = tmp;
	ft_export(fake_argv, micli);
	free(tmp);
}

void	update_pwd(t_micli *micli)
{
	const char	*pwd;
	char		path[PATH_MAX];
	const char	*current;
	char		*tmp;
	const char	**fake_argv;

	current = getcwd(path, sizeof(path));
	pwd = "PWD=";
	tmp = ft_strjoin(pwd, current);
	fake_argv = clean_calloc(3, sizeof(char *), micli);
	fake_argv[1] = tmp;
	ft_export(fake_argv, micli);
	tmp = ft_del(tmp);
	fake_argv = ft_del(fake_argv);
}

/*
** OLDPWD rewrite.
*/

void	delete_oldpwd(t_micli *micli)
{
	char	**fake_argv;
	char	*oldpwd;

	oldpwd = find_var("OLDPWD", 6, micli->envp);
	fake_argv = clean_calloc(3, sizeof(char *), micli);
	fake_argv[1] = "OLDPWD";
	if (oldpwd)
		ft_unset(fake_argv, micli);
	ft_export((const char **)fake_argv, micli);
	fake_argv = ft_del(fake_argv);
}
