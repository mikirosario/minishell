/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 10:26:59 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/12 21:22:45 by mrosario         ###   ########.fr       */
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
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
#include "libft.h"
#include <signal.h>

#define READLINE_BUFSIZE 1024
#define BUILTINS "exit,pwd,export,env,echo,cd,unset"
//#define	CMDLINE_END ";|"
#define DQUOTE_ESC_CHARS "\"$\\"
#define PIPE_MAX __SIZE_MAX__ / 2
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
	char			*var_flag;
	unsigned char	quote_flag:2; //This flag has 2 bits. First bit is single quotes, second bit is double quotes. 00 = quotes closed, 01 = double quotes open single quotes closed, 10 = single quotes open double quotes closed, 11 = double and single quotes open.
	unsigned char	redirect_flag:2; //This flag has 2 bits. 00 = No redirect 01 = write to file, trunc. 10 = write to file, append 11 = read from file.
	unsigned char	escape_flag:1; //This flag has 1 bit. If it is set, the following character has been 'escaped' and should be read as a character rather than an operator.
	unsigned char	cmd_flag:1; //This flag has 1 bit. If it is set, the command has been tokenized, thus all subsequent tokens are arguments
	//unsigned char	var_flag:1; //This flag has 1 bit. If it is set, the following character string until the next space is a variable, which will be resolved before continuing.
}				t_tokendata;

typedef struct	s_token
{
	t_list	*var_lst;
}				t_token;

typedef struct	s_cmdline
{
	char			*cmd;
	t_list			*arguments;
	//t_list		*redir_tokens; //Obsoleted by new code
	t_list			*redir_out;
	t_list			*redir_in;
	char			*redir_start;
	char			*redir_end; //This flag saves the address of the character after the redirect instruction set ends; all preceding characters are sequestered from the argument list
	char			**micli_argv;
	int				fd_redir_in;
	int				fd_redir_out;
	unsigned char	redir_in_flag:1; //This flag will indicate whether or not to pereform a redirect input instruction
	unsigned char	redir_out_flag:2; //This flag will indicate whether a redirect output instruction will truncate or append file. 00 no redir, 01 trunc, 10 append.
}				t_cmdline;


// typedef struct	s_varlist
// {
// 	char				*varname;
// 	char				*varcontent;
// 	struct s_varlist	*next;
// }				t_varlist;

typedef struct	s_builtins
{
	int 		argflag;
}				t_builtins;

typedef struct	s_pipes
{
	int		*array;
	size_t	array_size;
	size_t	count;
	size_t	cmd_index;
}				t_pipes;


typedef struct	s_micli
{
	t_tokendata		tokdata;
	t_cmdline		cmdline;
	t_token			token;
	t_builtins		builtins;
	t_pipes			pipes;
	pid_t			pid; //child process exist
	//size_t		builtin_strlen;
	//size_t		position; obsoleted by new code implementation
	//size_t		bufsize; obsoleted by new code implementation
	int				pipe[6]; //three-pipe array
	int				c;
	int				syserror;
	int				cmd_result;
	char			*cmd_result_str;
	char			**envp;
	char			*buffer;
	char			*tmp;
	unsigned char	quote_flag:1; //Raw_line quote flag...
	unsigned char	pipe_flag:2;
	unsigned char	pipe_reset_flag:2; //Pipe controls which pipe in the pipe array needs to be reopened for following exec cycle
}				t_micli;

/* Command Execution */

void	exec_cmd(char *cmd, t_list *arglst, t_micli *micli);

/* Flag Handling */
unsigned char	toggle_quote_flag(char quotes, unsigned char quote_flag);
unsigned char	toggle_pipe_flag(char pipe, unsigned char pipe_flag);

/* String Parsing */
int		isvarchar(char chr);
char	*find_var(const char *name, size_t name_len, char **envp);
size_t	get_var_lengths(t_list *var_lst);
void	process_raw_line(char *line, t_micli *micli);
char	process_char(char *chr, t_micli *micli);

/* Copying */

char *micli_cpy(char *dst, const char *src, char *src_end, t_micli *micli);

/* Concurrent Pipe Handling */
size_t	pipe_count(const char *line, t_micli *micli);
int		pipe_reset(t_pipes *pipes, t_micli *micli);

// /* Sequential Pipe Handling */
// int		pipe_reset(unsigned char pipe_reset_flag, int *pipe);
// void	close_write_end_preceding_pipe(unsigned char pipe_reset_flag, int *pipes);
// void	pipe_selector(unsigned char pipe_reset_flag, int *writepos, int *readpos);
// size_t	pipe_count(const char *line, t_micli *micli);

/* Memory Freeing */

t_list	*ft_lstfree(t_list *lst);
void	freeme(t_micli *micli);
void	clear_cmdline(t_micli *micli);
char	**free_split(char **split); //Move to libft

/* Memory Reservation */
char	**ft_envdup(char **envp, t_micli *micli);
char	*var_alloc(char *var_name, t_micli *micli);
char	*clean_ft_strdup(char const *str, t_micli *micli);
char	*clean_ft_strjoin(char const *s1, char const *s2, t_micli *micli);
char	**clean_ft_split(const char *s, char c, t_micli *micli);
void	*ft_realloc(void *ptr, size_t size, t_micli *micli);
void	*clean_calloc(size_t count, size_t size, t_micli *micli);

/* Signal Call */
void	catch_signal(int signum);
void 	waiting(int signum);
void	sigrun(int sig);

// void	do_nothing(int nb);
// void	signal_handler(int nb);

// void	signal_d();

/* Builtins */

int		exec_builtin(char *cmd, t_micli *micli);
int     ft_cd(const char **argv, t_micli *micli);
int    	ft_pwd(const char **argv);
void	oldpwd(t_micli *micli);
void	update_pwd(t_micli *micli);
void	delete_oldpwd(t_micli *micli);
int	    ft_echo(const char **argv, t_micli *micli);
int	    ft_unset(char **argv, t_micli *micli);
int		ft_env(char **envp, const char **argv);

/* Export */
int		ft_export(const char **argv, t_micli *micli);
char	find_pos(const char *name, size_t name_len, char **envp);
size_t	ft_name_len(const char *str);
size_t	ft_countarr(char **envp);
int		export_print(t_micli *micli);
void	export_order(t_micli *micli);
void	new_var(const char **argv, size_t str_len, t_micli *micli, int z);
void	upd(const char **argv, size_t name_len, t_micli *micli, int z);
int		var_check(const char *str);



/* Redirections */
char *find_redir_end(char *redir_str);


/* Exit Handling */

void	exit_success(t_micli *micli);
void	exit_failure(t_micli *micli);

/* Error Handling */
int		syntax_check(char *line);
int		print_error(char *error, char *error_location);

#endif