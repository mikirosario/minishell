/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 10:26:59 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/08 17:33:48 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _MINISHELL_
# define _MINISHELL_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
//#include <sys/types.h>
//#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
#include "libft.h"

#define READLINE_BUFSIZE 1
#define BUILTINS "exit,cd,echo,pwd,export,unset,env"
#define DEL 127

typedef struct	s_tokendata
{
	size_t			toksize;
	size_t			args;
	char			*tok_start;
	char			*tok_end;
	unsigned char	quote_flag:2; //This flag has 2 bits. First bit is single quotes, second bit is double quotes. 00 = quotes closed, 01 = double quotes open single quotes closed, 10 = single quotes open double quotes closed, 11 = double and single quotes open.
	unsigned char	escape_flag:1; //This flag has 1 bit. If it is set, the following character has been 'escaped' and should be read as a character rather than an operator.
	unsigned char	cmd_flag:1; //This flag has 1 bit. If it is set, the command has been processed, thus all subsequent tokens are arguments
	char			quote;
}				t_tokendata;

typedef struct	s_token
{
	char	*cmd;
	t_list	*arguments;
	char	**micli_argv;
}				t_token;

typedef struct	s_varlist
{
	char				*varname;
	char				*varcontent;
	struct s_varlist	*next;
}				t_varlist;

typedef struct	s_micli
{
	t_tokendata	tokdata;
	t_token		*token;
	size_t		builtin_strlen;
	int			position;
	int			bufsize;
	int			c;
	int			syserror;
	int			cmd_result;
	char		**envp;
	char		*buffer;
	char		*tmp;
}				t_micli;

/* Command Execution */
void	exec_cmd(char *cmd, t_list *arglst, t_micli *micli);

/* Flag Handling */
unsigned char	toggle_quote_flag(char quotes, char quote_flag);

/* String Parsing */
void	tokenize(char *line, t_micli *micli);

/* Memory Freeing */
void	freeme(t_micli *micli);
void	free_token(t_micli *micli);
char	**free_split(char **split); //Move to libft

/* Memory Reservation */
char	*clean_ft_strdup(char const *str, t_micli *micli);
char	*clean_ft_strjoin(char const *s1, char const *s2, t_micli *micli);
char	**clean_ft_split(const char *s, char c, t_micli *micli);
char	*ft_realloc(char *ptr, size_t size, t_micli *micli);
void	*clean_calloc(size_t count, size_t size, t_micli *micli);

/* Exit Handling */

void	exit_success(t_micli *micli);
void	exit_failure(t_micli *micli);

/* Signal Call */

void	signal_c();
void	singal_barra();
void	signal_d();

/* Builtins */

int		cd(const char **argv, char **envp, t_micli *micli);
void	pwd();

#endif