/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_window_size.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/28 00:49:37 by miki              #+#    #+#             */
/*   Updated: 2021/03/28 00:51:04 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	get_window_size(int *rows, int *cols)
{
	struct winsize tmp;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &tmp) == -1 || (tmp.ws_col == 0 || tmp.ws_row == 0))
		return (0);
	*rows = tmp.ws_row;
	*cols = tmp.ws_col;
	return (1);
}
