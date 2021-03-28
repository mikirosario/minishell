/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 10:26:59 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/28 20:41:51 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <fcntl.h>
# include <limits.h>
# include <errno.h>
# include <string.h>
# include <dirent.h>
# include <sys/wait.h>
# include <signal.h>
# include <termios.h>
# include <sys/ioctl.h>
# include "libft.h"

# define READLINE_BUFSIZE 10
# define CMDHIST_BUF 20
# define BUILTINS "exit,pwd,export,env,echo,cd,unset"
# define DQUOTE_ESC_CHARS "\"$\\"
# define DEL 127
# define SUB 26
# define NUL ""
# define SYN_ERROR "micli: syntax error near unexpected token"
# define BUF_OVERFLOW "\nBAD BUNNY! TOO MANY CHARACTERS IN LINE BUFFER!"

typedef struct s_tokendata
{
	size_t			toksize;
	size_t			args;
	char			**path_array;
	char			*tok_start;
	char			*tok_end;
	char			*var_flag;
	unsigned char	quote_flag : 2;
	unsigned char	redirect_flag : 2;
	unsigned char	escape_flag : 1;
	unsigned char	cmd_flag : 1;
}				t_tokendata;

typedef struct s_token
{
	t_list	*var_lst;
}				t_token;

typedef struct s_cmdline
{
	char			*cmd;
	t_list			*arguments;
	t_list			*redir_out;
	t_list			*redir_in;
	char			*redir_start;
	char			*redir_end;
	char			**micli_argv;
	int				fd_redir_in;
	int				fd_redir_out;
	unsigned char	redir_in_flag : 1;
	unsigned char	redir_out_flag : 2;
}				t_cmdline;

typedef struct s_cmdhist
{
	short			**hist_stack;
	size_t			active_line_size;
	size_t			cmdhist_buf;
	size_t			ptrs_in_hist;
	size_t			index;
}				t_cmdhist;

typedef struct s_builtins
{
	int	argflag;
}				t_builtins;

typedef struct s_pipes
{
	int				*array;
	size_t			array_size;
	size_t			count;
	size_t			cmd_index;
}				t_pipes;

typedef struct s_normis_fault
{
	size_t	pipe_max;
	mode_t	perms;
	int		f_tr;
	int		f_ap;
	int		f_re;
}				t_normis_fault;

typedef struct s_micli
{
	t_cmdhist		cmdhist;
	t_tokendata		tokdata;
	t_cmdline		cmdline;
	t_token			token;
	t_builtins		builtins;
	t_pipes			pipes;
	t_normis_fault	tonti;
	struct termios	orig_term;
	struct termios	raw_term;
	int				syserror;
	int				cmd_result;
	char			*cmd_result_str;
	char			**envp;
	char			*buffer;
	short			*active_line;
	unsigned char	quote_flag : 1;
	unsigned char	pipe_flag : 2;
}				t_micli;

/*
** Short String Functions
*/

size_t			ft_strlen16(short *str);
char			*short_to_chars(short short_char);
char			*ft_short_to_strdup(short *short_str);

/*
** Main Loop
*/

size_t			del_from_buf(short *chr, size_t num_chars);
char			micli_loop(t_micli *micli);

/*
** Termcaps
*/

void			enable_raw_mode(struct termios *raw_term, \
				struct termios *orig_term);
char			is_esc_seq(short *buf, short *char_total, char *move_flag);

/*
** Command History
*/

void			cmdhist_ptr_array_alloc(t_micli *micli, t_cmdhist *cmdhist);
void			push_to_hist_stack(t_micli *micli, short *active_line, \
				t_cmdhist *cmdhist);

/*
** Command Execution
*/

char			*find_cmd_path(char *cmd, const char *paths, t_micli *micli);
int				get_child_exit_status(int stat_loc);
void			exec_cmd(char *cmd, t_list *arglst, t_micli *micli);
void			exec_child_process(char *exec_path, char *builtin, char *cmd, \
				t_micli *micli);

/*
** Flag Handling
*/

unsigned char	toggle_quote_flag(char quotes, unsigned char quote_flag);
unsigned char	toggle_pipe_flag(char pipe, unsigned char pipe_flag);

/*
** String Parsing
*/

int				is_escape_char(char chr, char next_chr, \
				unsigned char escape_flag, unsigned char quote_flag);
int				is_quote_char(char chr, unsigned char escape_flag, \
				unsigned char quote_flag);
int				is_cmdline(char chr, unsigned char escape_flag, \
				unsigned char quote_flag);
int				is_variable_start(char *chr, t_tokendata *tokdata, \
				t_micli *micli);
int				is_variable_end(char *chr, char *end_var_addr);
int				is_redirect_start(char chr, unsigned char escape_flag, \
				unsigned char quote_flag, char *redir_end);
void			escape_operations(char *chr, t_micli *micli);
void			quote_operations(char *chr, t_micli *micli);
void			redir_start_operations(char *chr, t_micli *micli);
void			redir_end_operations(t_micli *micli);
void			variable_start_operations(char *chr, t_micli *micli);
int				isvarchar(char chr);
char			*find_var(const char *name, size_t name_len, char **envp);
size_t			get_var_lengths(t_list *var_lst);
void			process_raw_line(char *line, t_micli *micli);
int				process_cmdline(char *startl, char *endl, t_micli *micli);
void			process_token(t_micli *micli);
char			process_char(char *chr, t_micli *micli);

/*
** Copying
*/

char			*micli_cpy(char *dst, const char *src, char *src_end, \
				t_micli *micli);

/*
** Pipe Handling
*/

size_t			pipe_count(const char *line);
void			clear_pipes(t_pipes *pipes, t_micli *micli);
int				pipe_reset(t_pipes *pipes, t_micli *micli);

/*
** Redirections
*/

void			interpret_redir_operator(const char *redir, t_micli *micli);
void			open_redir_file(t_normis_fault *tonti, t_micli *micli);
char			*find_redir_end(char *redir_str);

/*
** Exit Handling
*/

void			exit_success(t_micli *micli);
void			exit_failure(t_micli *micli);

/*
** Error Handling
*/

int				syntax_check(char *line, t_micli *micli);
int				print_error(char *error, char *error_location, t_micli *micli);
void			sys_error(t_micli *micli);
int				broken_pipe_check(pid_t pid);

/*
** Memory Freeing
*/

t_list			*ft_lstfree(t_list *lst);
void			freeme(t_micli *micli);
void			clear_cmdline(t_micli *micli);

/*
** Memory Reservation
*/

char			**ft_envdup(char **envp, t_micli *micli);
char			*var_alloc(char *var_name, t_micli *micli);
char			*clean_ft_strdup(char const *str, t_micli *micli);
void			*clean_ft_memdup(void const *mem, size_t memsize, \
				t_micli *micli);
char			*clean_ft_strjoin(char const *s1, char const *s2, \
				t_micli *micli);
char			**clean_ft_split(const char *s, char c, t_micli *micli);
void			*clean_realloc(void *ptr, size_t new_size, size_t old_size, \
				t_micli *micli);
void			*clean_calloc(size_t count, size_t size, t_micli *micli);

/*
** Signal Call
*/

void			catch_signal(int signum);
void			waiting(int signum);
void			sigrun(int sig);
void			sigquit(int signum);

/*
** Builtins
*/

int				exec_builtin(char *cmd, t_micli *micli);

/*
** cd
*/

int				ft_cd(const char **argv, t_micli *micli);
int				cd_helper(const char **argv, t_micli *micli);
void			oldpwd(t_micli *micli);
void			update_pwd(t_micli *micli);
void			delete_oldpwd(t_micli *micli);
int				ft_pwd(const char **argv);

/*
** echo
*/

int				ft_echo(const char **argv, t_micli *micli);
void			echo_helper(int i, const char **argv, t_micli *micli);

/*
** unset
*/

int				ft_unset(char **argv, t_micli *micli);
void			unset_helper(char *store, t_micli *micli);
int				ft_env(char **envp, const char **argv);

/*
** Export
*/

int				ft_export(const char **argv, t_micli *micli);
int				var_op(const char **argv, t_micli *micli, int z);
char			find_pos(const char *name, size_t name_len, char **envp);
size_t			ft_name_len(const char *str);
size_t			ft_countarr(char **envp);
void			export_print(t_micli *micli);
size_t			*export_order(t_micli *micli);
void			exp_order_h(size_t countarr, size_t *mask, t_micli *micli);
void			new_var(const char **argv, size_t str_len, t_micli *micli, \
				int z);
void			upd(const char **argv, size_t name_len, t_micli *micli, int z);
int				var_check(const char *str);

#endif
