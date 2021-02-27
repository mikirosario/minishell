/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/27 17:16:20 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/27 19:05:36 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


// size_t	ptr_array_len(char **ptr_array)
// {
// 	size_t i;

// 	i = 0;

// 	while (ptr_array[i])
// 		i++;
// 	return (i);
// }

//int		copy_envp(char **new_envp, char **old_envp, t_micli *micli)


// char	**create_new_charptr_array(size_t count, t_micli *micli)
// {
// 	return (clean_calloc(count + 1, sizeof(char *), micli));
// }

int		replace_envp_var(char *var_name, char *replacement_var, t_micli *micli)
{
	char *replace_var;
	char *old_var;

	if (!(replace_var = find_var(var_name, ft_strlen(var_name), micli->envp)))
		return (0);
	old_var = replace_var;
	replace_var = clean_ft_strdup(replacement_var, micli);
	old_var = ft_del(old_var);
	return (1);
}

int     cd(const char **argv, t_micli *micli) 
{
    char *prefix;
	char *new_pwd;
	char *cwd;

	prefix = NULL;
	cwd = NULL;
	new_pwd = "PWD=";
	// char **old_envp;

	// old_envp = micli->envp;
	// micli->envp = create_new_charptr_array(ptr_array_len(old_envp), micli);

	if (!argv[1]) //Must get argc...
		return (1);
	if (*argv[1] == '~')
	{
		//prefix = (find_var("HOME", 4, micli->envp) + 5); //for ~ prefix is the string that starts after HOME= in envp
		printf("BLAH\n");
		return (1);
	}
	else
	{
        if (chdir(argv[1]) == -1)
        {
            ft_printf("cd: %s: %s\n", argv[1], strerror(errno));
            return(1);
        }
		else
		{
			cwd = (getcwd(cwd, 0));
			new_pwd = clean_ft_strjoin(new_pwd, cwd, micli);
			cwd = ft_del(cwd);
			replace_envp_var("PWD", new_pwd, micli);
		}
		return(0);
	}
    //simple change directory if first argument exist
    // if(argv[1] == 0 || *argv[1] == '~')
    // {
    //     // 0 1 2 3 4 5
    //     // H O M E = /BLABLA
    //     //ft_printf("%s%s: ", home, (argv[1] + 2), strerror);
    //     home = find_var("HOME", 4, micli->envp);
    //     chdir(home += 5);
    //     //error habndling
    //     return(0);
    // }

    if (argv[1] != 0 && *argv[1] != '~')
    {
        if (chdir(argv[1]) == -1)
        {
            ft_printf("cd: %s: %s\n", argv[1], strerror(errno));
            return(1);
        }
        else
            return(0);
    }
    return(0);
    // here should update env pwd and old pwd, (where are you and where have you been)
    //
    // 
}