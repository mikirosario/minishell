/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   termcaps.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/01 10:52:33 by miki              #+#    #+#             */
/*   Updated: 2021/04/01 20:19:15 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function prints error messages and calls exit_failure if the termcaps
** init process fails.
*/

void	termcaps_init_fail(t_micli *micli, int failure, char *termtype)
{
	micli->syserror = errno;
	if (!termtype || !*termtype)
		printf("💥 TERM is unset. Set a terminal type: export TERM=<type>\n");
	else if (failure == 0)
		printf("💥 Terminal type '%s' not found in database.\n", termtype);
	else if (failure == -1)
		printf("💥 Could not access the termcap database.\n");
	else if (failure == -2)
		printf("💥 Your terminal lacks required basic capabilities...\n");
	else
		return ;
	exit_failure(micli);
}

/*
** For this character deletion settings algorithm I've followed the advice in
** GNU documentation. :) The idea is we check to see if there is any way to
** delete a character in the terminal. If there isn't... well... that's pretty
** basic, so that is unacceptable.
**
** Some terminals have a special delete mode and exit_delete mode, so we check
** for this. If the delete mode is the same as insert mode that is fine, the
** insert mode string will be cloned automatically by tgetstr for that terminal.
** Similarly, exit_delete may either be its own string or a clone of delete_mode
** and tgetstr will worry about it. That works for me, I have more than enough
** to worry about.
*/

int	character_delete_caps(t_termcaps *tcaps, char **capbuf)
{
	tcaps->delcaps.delete_char = tgetstr("dc", capbuf);
	if (!tcaps->delcaps.delete_char)
		return (0);
	tcaps->delcaps.delete_mode = tgetstr("dm", capbuf);
	tcaps->delcaps.exit_delete = tgetstr("ed", capbuf);
	return (1);
}

/*
** For this character insertion settings algorithm I've followed the advice in
** the GNU documentation. :) The idea is we check to make sure the terminal has
** *some* way to insert characters, whether that be by activating an insert mode
** or with a special command. If it has an insert mode and a special command we
** prefer the insert mode. If there is no insert mode but there is a special
** command we define the special insert_char command, otherwise we define the
** exit mode. This must be present if there is an insert mode, even if it is
** just a toggle (insert mode repeated). We try to define padding as well.
**
** At least one of the above MUST be defined for the shell to be able to work
** with the terminal.
**
** We also define the mi flag which says whether or not insert mode persists
** after you move the cursor, though I avoid using this at the moment, and the
** spc_not_clr_flag, which is my name for "in", which is set when the terminal
** distinguishes between character positions with a space and character
** positions that have been cleared.
**
** Ref:
** https://www.gnu.org/software/termutils/manual/termcap-1.3/html_chapter/
** termcap_4.html#SEC32
*/

int	character_insertion_caps(t_termcaps *tcaps, char **capbuf)
{
	tcaps->inscaps.insert_mode = tgetstr("im", capbuf);
	if (!tcaps->inscaps.insert_mode)
		tcaps->inscaps.insert_char = tgetstr("ic", capbuf);
	else
		tcaps->inscaps.exit_insert = tgetstr("ei", capbuf);
	tcaps->inscaps.insert_pad = tgetstr("ip", capbuf);
	tcaps->inscaps.mov_while_ins_flag = tgetflag("mi");
	tcaps->inscaps.spc_not_clr_flag = tgetflag("in");
	if (tcaps->inscaps.insert_mode || tcaps->inscaps.insert_char \
	 || tcaps->inscaps.insert_pad)
		return (1);
	return (0);
}

/*
** The following capabilities are mandatory by order of the King of Spain:
**
** - cur_up, we won't get very far without being able to move up, eh?
** - cur_left, can you imagine a terminal without being able to move left?
** - cur_right, moving left and right is pretty basic, yes?
** - carriage_ret, this feature dates back to the XIX century, no excuses!
** - del_line, this is more convenience for me, but I'm worth it!
** - arrow_up, this is the arrow up sequence in keypad mode, used for scroll
** - arrow_down, this is the arrow down sequence in keypad mode
**
** The strlen condition is a bit silly, but I originally programmed this to
** handle the standard vt100 ANSI codes and then adapted for termcaps, so,
** basically, the parser is not designed to handle arrow escape sequences that
** aren't exactly 3 charactesr long. xD I want to avoid rewriting the thing to
** be more flexible, and really I only need it to work with keypad mode vt100,
** which is also 3 characters, so I just ban sequences of any other length. If
** there ARE any novel 3-character sequences that aren't \eOA and \eOB, though,
** this minishell SHOULD work with them. ;)
**
** I use derive esc_seq to get the initial two-character short for the up and
** down arrows, and I check the third character in the saved sequences for the
** id. It is all really rather silly and it seems to me this stuff could just be
** hardcoded, BUT, it is OBLIGATORY to use termcaps with the arrow keys, so I am
** using termcaps with the arrow keys... :p
*/

int	misc_mandatory_caps(t_termcaps *tcaps, char **capbuf)
{
	char	*tmp;

	tmp = tgetstr("pc", capbuf);
	tcaps->cur_up = tgetstr("up", capbuf);
	tcaps->cur_left = tgetstr("le", capbuf);
	tcaps->cur_right = tgetstr("nd", capbuf);
	tcaps->carriage_ret = tgetstr("cr", capbuf);
	tcaps->del_line = tgetstr("dl", capbuf);
	tcaps->arrow_up = tgetstr("ku", capbuf);
	tcaps->arrow_left = tgetstr("kl", capbuf);
	tcaps->arrow_right = tgetstr("kr", capbuf);
	tcaps->arrow_down = tgetstr("kd", capbuf);
	derive_esc_seq(tcaps);
	if (!tcaps->cur_up || !tcaps->cur_left || !tcaps->cur_right \
	 || !tcaps->carriage_ret || !tcaps->del_line || !tcaps->arrow_up \
	 || !tcaps->arrow_down || ft_strlen(tcaps->arrow_up) != 3 \
	 || ft_strlen(tcaps->arrow_down) != 3)
		return (0);
	return (1);
}

/*
** First we use find_var to find the TERM environment variable. The find_var
** function is my version of getenv, but unlike getenv it returns a pointer to
** the key, rather than the value, so if the key is found we need to also skip
** it and make sure the value exists after the key. If either of these checks
** fail... that's a paddling.
**
** If a termtype is found at the TERM variable, we try to load its associated
** database into the term buffer with tgetent. If that fails it will return 0 if
** the database could not be found, or -1 if it was found but could not be
** accessed. For the former we record failure condition 0 and for the latter
** failure condition -1.
**
** If crucial capabilities are found lacking in the terminal we report failure
** condition -2.
**
** Finally we will try to get the terminal attributes with tcgetattr. If this
** fails we report failure condition -3.
**
** We send all the results to termcaps_init_fail. If any failure happened along
** the way, it will print the appropriate error message and abort the program.
**
** DEBUG CODE TO WORK WITH VSCODE:
	// //DEBUG REMOVE FROM FINAL
	// char	*fake_argv[2];
	// if (!termtype)
	// {
	// 	fake_argv[0] = "export";
	// 	fake_argv[1] = "TERM=xterm-256color";
	// 	ft_export((const char **)fake_argv, micli);
	// 	termtype = find_var("TERM", 4, micli->envp);
	// 	printf("TERM: %s\n", termtype);
	// }
	// //DEBUG REMOVE FROM FINAL
*/

void	termcaps_init(t_micli *micli, t_termcaps *tcaps)
{
	char	*termtype;
	char	*capbuf;

	capbuf = &tcaps->capbuf[0];
	termtype = find_var("TERM", 4, micli->envp);
	if (termtype)
	{
		termtype += 5;
		if (*termtype)
			tcaps->init_result = tgetent(tcaps->termbuf, termtype);
	}
	if (tcaps->init_result > 0)
		if (!character_insertion_caps(tcaps, &capbuf) \
		 || !character_delete_caps(tcaps, &capbuf) \
		 || !misc_mandatory_caps(tcaps, &capbuf))
			tcaps->init_result = -2;
	termcaps_init_fail(micli, tcaps->init_result, termtype);
}
