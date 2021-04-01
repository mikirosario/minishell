/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/24 18:17:50 by mrosario          #+#    #+#             */
/*   Updated: 2021/04/01 11:06:19 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/stat.h>

/*
** This function is the poster child of things that only exist because of the
** school norm. In this case, the school norm prohibits using operations to
** define constants in the header like normal human beings. Actually, it doesn't
** even recognize constants as constants if you do.
**
** So I have no choice but to define all my constants that are a result of maths
** or bitwise operations here, at run time. All variable, structure and function
** names are in honour of Norminette, French feline and royal pain in the arse.
**
** *le eyeroll*
*/

static void	norminette_made_me_do_it(t_micli *micli)
{
	ft_bzero(&micli->tonti, sizeof(t_normis_fault));
	micli->tonti.pipe_max = __SIZE_MAX__ / 2;
	micli->tonti.perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	micli->tonti.f_tr = O_WRONLY | O_CREAT | O_TRUNC;
	micli->tonti.f_ap = O_WRONLY | O_CREAT | O_APPEND;
	micli->tonti.f_re = O_RDONLY | O_CREAT;
}

/*
** This is the main loop of the program. The first thing it does is to enable
** raw mode. This deactivates canonical tty mode and gives us more direct
** control of keyed input, which we need for the command line history feature.
**
** We set the SIGINT and SIGQUIT signals to trigger the sigrun function. We
** write the prompt to the terminal. Then we go into micli_readline, which will
** set up a readline buffer and read all input from STDIN to the buffer.
**
** The function micli_readline will return its readline_buffer when it receives
** a \n character, and we save the address of the generated buffer at
** micli_active_line. We then make a duplicate of it using clean_ft_memdup and
** save its address to micli->active line. Don't worry, we haven't lost the
** reference to the original... there is another. ;)
**
** Then we call push_to_hist_stack. This function will take the active line and
** store it at the top of the the command line history stack as an entry, so the
** user can scroll back up to it. The hist_stack is a pointer to short array.
**
** Now we need to to send this command line to our parser, which is not designed
** to parse through a short string, but rather through a normal char string.
** That is not a problem though. We pass the active_line short string to the
** function ft_short_to_strdup, which converts a null-terminated string made of
** shorts into a null-terminated string made of chars. (If it's just a null char
** we just use ft_strdup with a literal).
**
** Now we're done with active line and we free it.
**
** Then we use tcsetattr to restore the canonical mode.
** 
*/

char	micli_loop(t_micli *micli)
{
	while (1)
	{
		enable_raw_mode(&micli->raw_term, &micli->orig_term);
		signal(SIGINT, sigrun);
		signal(SIGQUIT, sigrun);
		write(STDOUT_FILENO, "🚀 ", 5);
		micli->active_line = micli_readline(micli, &micli->cmdhist, \
		micli->cmdhist.hist_stack);
		micli->active_line = clean_ft_memdup(micli->active_line, \
		(micli->cmdhist.recalc_bufsize + 3) * sizeof(short), micli);
		push_to_hist_stack(micli, micli->active_line, &micli->cmdhist);
		if (micli->active_line[2] == 0)
			micli->buffer = clean_ft_strdup("\0", micli);
		else
			micli->buffer = ft_short_to_strdup(&micli->active_line[2]);
		micli->active_line = ft_del(micli->active_line);
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &micli->orig_term);
		process_raw_line(micli->buffer, micli);
		micli->buffer = ft_del(micli->buffer);
	}
	return (0);
}

/*
** Eh! No TTY! If it's not a TTY, <Uncle Roger>FAILURE</Uncle Roger>.
*/

void	eh_notty(t_micli *micli)
{
	if (!isatty(STDIN_FILENO))
	{
		micli->syserror = ENOTTY;
		exit_failure(micli);
	}
	if (tcgetattr(STDIN_FILENO, &micli->orig_term) == -1)
	{
		micli->syserror = errno;
		exit_failure(micli);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_micli	micli;

	(void)argc;
	(void)argv;
	printf("\033[0;32m		               __       42Madrid\n");
	printf("\033[0;32m		  __ _  (_)___/ (_) 	Mar 2021\n");
	printf("\033[0;32m		 /  ' \\/ / __/ / /  	mrosario\n");
	printf("\033[0;32m		/_/_/_/_/\\__/_/_/   	mvillaes\033[0m\n");
	ft_bzero(&micli, sizeof(t_micli));
	micli.cmdhist.hist_stack = clean_calloc(2, sizeof(short *), &micli);
	micli.cmdhist.ptrs_in_hist = 1;
	micli.cmdhist.cmdhist_buf = 1;
	micli.envp = ft_envdup(envp, &micli);
	eh_notty(&micli);
	norminette_made_me_do_it(&micli);
	delete_oldpwd(&micli);
	if (LINUX == 1)
		linux_compatibility_mode(&micli);
	termcaps_init(&micli, &micli.termcaps);
	micli_loop(&micli);
	return (0);
}
