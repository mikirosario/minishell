/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 10:26:59 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/12 18:06:21 by mrosario         ###   ########.fr       */
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
#define BUILTINS "exit,cd,echo,pwd,unset,env"
#define DEL 127
#define SUB 26
#define NUL ""
#define SYN_ERROR "micli: syntax error near unexpected token"

typedef struct	s_tokendata
{
	size_t			toksize;
	size_t			args;
	char			**path_array;
	char			*tok_start;
	char			*tok_end;
	unsigned char	quote_flag:2; //This flag has 2 bits. First bit is single quotes, second bit is double quotes. 00 = quotes closed, 01 = double quotes open single quotes closed, 10 = single quotes open double quotes closed, 11 = double and single quotes open.
	unsigned char	escape_flag:1; //This flag has 1 bit. If it is set, the following character has been 'escaped' and should be read as a character rather than an operator.
	unsigned char	cmd_flag:1; //This flag has 1 bit. If it is set, the command has been tokenized, thus all subsequent tokens are arguments
	unsigned char	var_flag:1; //This flag has 1 bit. If it is set, the following character string until the next space is a variable, which will be resolved before continuing.
}				t_tokendata;

typedef struct	s_token
{
	t_list	*var_lst;
}				t_token;

typedef struct	s_cmdline
{
	char	*cmd;
	t_list	*arguments;
	char	**micli_argv;
}				t_cmdline;


typedef struct	s_varlist
{
	char				*varname;
	char				*varcontent;
	struct s_varlist	*next;
}				t_varlist;

typedef struct	s_builtins
{
	int 		argflag;
}				t_builtins;

typedef struct	s_micli
{
	t_tokendata	tokdata;
	t_cmdline	*cmdline;
	t_token		*token;
	t_builtins	builtins;
	//size_t		builtin_strlen;
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
char	*find_var(char *name, size_t name_len, char **envp);
size_t	get_var_lengths(t_list *var_lst);
void	process_raw_line(char *line, t_micli *micli);

/* Copying */
char *micli_cpy(char *dst, const char *src, char *src_end, t_micli *micli);

/* Memory Freeing */
t_list	*ft_lstfree(t_list *lst);
void	freeme(t_micli *micli);
void	clear_cmdline(t_micli *micli);
char	**free_split(char **split); //Move to libft

/* Memory Reservation */
int		var_alloc(char *var_name, t_micli *micli);
char	*clean_ft_strdup(char const *str, t_micli *micli);
char	*clean_ft_strjoin(char const *s1, char const *s2, t_micli *micli);
char	**clean_ft_split(const char *s, char c, t_micli *micli);
char	*ft_realloc(char *ptr, size_t size, t_micli *micli);
void	*clean_calloc(size_t count, size_t size, t_micli *micli);

/* Signal Call */

int		catch_signal();
void	ctrl_c(int signum);
void	ctrl_bar(int signum);
// void	signal_c();
// void	singal_bar();
// void	signal_d();

/* Builtins */
void	exec_builtin(char *cmd, t_micli *micli);
int		ft_cd(const char **argv, char **envp, t_micli *micli);
void	ft_pwd(t_micli *micli);
void    ft_echo(const char **argv, t_micli *micli);

/* Exit Handling */
void	exit_success(t_micli *micli);
void	exit_failure(t_micli *micli);

/* Error Handling */
void	print_error(char *error, char *error_location);

#endif