/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/06 17:08:14 by mvillaes          #+#    #+#             */
/*   Updated: 2021/03/12 21:12:58 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_cd(const char **argv, t_micli *micli)
{
	char	*home;

	oldpwd(micli);
	if (argv[1] == NULL || (*argv[1] == '~' && (argv[1][1] == '/' || argv[1][1] == '\0')))
	{
		home = find_var("HOME", 4, micli->envp);
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
	{
		if (chdir(argv[1]) == -1)
		{
			ft_printf("cd: %s: %s\n", argv[1], strerror(errno));
			return (1);
		}
		update_pwd(micli);
		return (0);
	}
	return (0);
}

void	oldpwd(t_micli *micli)
{
	const char	*oldpwd;
	char		path[PATH_MAX];
	const char	*current;
	char		*tmp;
	const char	**fake_argv;
	size_t		str_len;

	current = getcwd(path, sizeof(path));
	oldpwd = "OLDPWD=";
	tmp = ft_strjoin(oldpwd, current);
	str_len = ft_strlen(tmp);
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
	size_t		str_len;

	current = getcwd(path, sizeof(path));
	pwd = "PWD=";
	tmp = ft_strjoin(pwd, current);
	str_len = ft_strlen(tmp);
	fake_argv = clean_calloc(3, sizeof(char *), micli);
	fake_argv[1] = tmp;
	ft_export(fake_argv, micli);
	free(tmp);
}

void	delete_oldpwd(t_micli *micli)
{
	const char	*oldpwd;
	char		*newold;
	const char	**fake_argv;
	int			findpos;

	oldpwd = "OLDPWD=";
	newold = "OLDPWD";
	findpos = find_pos(oldpwd, 7, micli->envp);
	free (micli->envp[findpos]);
	micli->envp[findpos] = NULL;
	fake_argv = clean_calloc(3, sizeof(char *), micli);
	fake_argv[1] = newold;
	ft_export(fake_argv, micli);
}
