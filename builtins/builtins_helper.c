#include "../minishell.h"

/*
** This function executes built-ins when they are called from shell.
*/

int	exec_builtin(char *cmd, t_micli *micli)
{
	if (!(ft_strcmp(cmd, "exit")))
		exit_success(micli);
	else if (!(ft_strcmp(cmd, "cd")))
		return (ft_cd((const char **)micli->cmdline.micli_argv, micli));
	else if (!(ft_strcmp(cmd, "pwd")))
		return (ft_pwd((const char**)micli->cmdline.micli_argv));
	else if (!(ft_strcmp(cmd, "echo")))
		return (ft_echo((const char **)micli->cmdline.micli_argv, micli));
	else if (!(ft_strcmp(cmd, "unset")))
		return (ft_unset(micli->cmdline.micli_argv, micli));
	else if (!(ft_strcmp(cmd, "env")))
		return (ft_env(micli->envp));
	else if (!(ft_strcmp(cmd, "export")))
		return (ft_export((const char **)micli->cmdline.micli_argv, micli));
	return (128);
}

char	find_pos(const char *name, size_t name_len, char **envp)
{
	int	i;

	i = 0;
	while (envp[i] != NULL && ft_strncmp(name, envp[i], name_len))
		i++;
	return (i);
}

size_t	ft_name_len(const char *str)
{
	int	i;

	i = 0;
	while (str[i] != 0 && str[i] != '=')
		i++;
	return (i);
}

void	export_order(t_micli *micli)
{
	int		i;
	int		j;
	char	countarr;
	char	*tmp_envp;

	i = 0;
	countarr = ft_countarr(micli->envp);
	while (i < countarr - 1)
	{
		j = 0;
		while (j < countarr - i - 1)
		{
			if (ft_strcmp(micli->envp[j], micli->envp[j + 1]) > 0)
			{
				tmp_envp = micli->envp[j];
				micli->envp[j] = micli->envp[j + 1];
				micli->envp[j + 1] = tmp_envp;
			}
			j++;
		}
		i++;
	}
}

/*
** char	*ft_comillas(const char *str, size_t name_len, size_t str_len)
** {
** 	char	*ret;

** 	ret = malloc(str_len + 3);
** 	ft_memcpy(ret, str, name_len + 1);
** 	ret[name_len + 1] = '"';
** 	ft_memcpy(ret + name_len + 2, str + name_len + 1, str_len - name_len - 1);
** 	ret[str_len + 1] = '"';
** 	ret[str_len + 2] = '\0';
** 	return (ret);
** }
*/