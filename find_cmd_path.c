/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_cmd_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 20:50:37 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/24 21:19:15 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	et_phone_home(t_micli *micli)
{	
	ft_printf("micli: %s: %s\n", micli->cmdline.cmd, strerror(2));
	micli_loop(micli);
}

/*
** This function finds comma-separated builtins in the BUILTIN constant string
** declared in the minishell.h header. If the argument passed as cmd matches a
** comma-separated builtin within the BUILTIN constant string, 1 is returned.
** Otherwise, 0 is returned. We need to compare against the string length of
** cmd, so since the BUILTIN string indicators are comma as well as
** null-terminated, we first check to make sure cmd is exactly the same length
** as the proposed built-in match before bothering to compare it.
**
** I'm assuming that after the pointer subtraction, size_t should be enough to
** hold the result until we become transhuman and routinely input 64 bit command
** names.
*/

int		find_builtin(char *cmd)
{
	char	*startl;
	char	*endl;
	size_t	cmd_strlen;

	startl = BUILTINS;
	endl = startl;
	cmd_strlen = ft_strlen(cmd);
	while (*startl)
	{
		while (*endl && *endl != ',')
			endl++;
		if (cmd_strlen == (size_t)(endl - startl) \
		&& !(ft_strncmp(startl, cmd, endl - startl)))
			return (1);
		else if (*endl == ',')
			endl++;
		startl = endl;
	}
	return (0);
}

/*
** This function generates the pathname of a binary after it is found within the
** binary directories included in the PATH environmental variable, which can
** then be passed to execve to execute the binary.
**
** The path to the directory is saved at the address pointed to by the path
** pointer without a trailing /, so the trailing / must be added in manually.
** That is why I created this function instead of just using str_join. :p
**
** The command name is saved at the address pointed to by the cmd pointer.
**
** First we determine the lengths of path and cmd. Then we reserve enough memory
** to hold both path and cmd, plus 2 bytes, one for the trailing / we need to
** put between path/cmd, and another for the terminating null.
**
** Then we copy path to the reserved memory block, add a '/' after path, and
** concatenate it with cmd. We return the result, which will be the pathname.
*/

char	*generate_pathname(char *path, char *cmd, t_micli *micli)
{
	char	*ret;
	size_t	pathlen;
	size_t	cmdlen;

	pathlen = ft_strlen(path);
	cmdlen = ft_strlen(cmd);
	ret = clean_calloc(pathlen + cmdlen + 2, sizeof(char), micli);
	ft_memcpy(ret, path, pathlen);
	ret[pathlen] = '/';
	ft_strlcat(ret, cmd, pathlen + cmdlen + 2);
	return (ret);
}

/*
** This function finds the path where the binary specified by cmd exists from
** among the directories specified within the PATH environmental variable. The
** The PATH variable is passed as the const char pointer paths.
**
** The first five characters of the PATH variable are PATH=, so we start
** searching from paths[5]:
**
**	0 1 2 3 4 5 start at pos 5
**	P A T H = / ...
**
** Directories in the PATH variable are divided by ':', so we use ft_split to
** isolate each directory in its own null-terminated string.
**
** First we compare cmd with a hardcoded constant string literal, BUILTINS,
** defined (currently) in minishell.h (might put this in a config file later).
**
** If cmd is in the BUILTIN string, we return the same address as the one cmd
** points to (micli->cmdline.cmd).
**
** If cmd is not found in BUILTIN we open all the directories in path_array one
** by one and check every entry in each directory one by one. If any of these
** entries matches cmd, we assemble a path name using the path where it was
** found (generate_pathname does this) and return an address to the path name
** thus generated.
**
** If no match is found either in BUILTINS or in the PATH directories, we return
** NULL.
*/

char	*find_cmd_path(char *cmd, const char *paths, t_micli *micli)
{
	DIR				*dir;
	struct dirent	*dirent;
	char			*ret;
	size_t			y;

	ret = NULL;
	y = 0;
	if (find_builtin(cmd))
		return (cmd);
	micli->tokdata.path_array = clean_ft_split(paths, ':', micli);
	if (!micli->tokdata.path_array)
		et_phone_home(micli);
	while (!ret && micli->tokdata.path_array[y])
	{
		dir = opendir(micli->tokdata.path_array[y]);
		if (dir)
			while ((dirent = readdir(dir)))
				if (!(ft_strncmp(dirent->d_name, cmd, ft_strlen(cmd) + 1)))
					ret = generate_pathname(micli->tokdata.path_array[y], \
					cmd, micli);
		if (dir)
			closedir(dir);
		y++;
	}
	if (micli->tokdata.path_array)
		micli->tokdata.path_array = ft_free_split(micli->tokdata.path_array);
	return (ret);
}
