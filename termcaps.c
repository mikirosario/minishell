/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   termcaps.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/20 20:51:11 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/21 11:23:18 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	is_esc_seq(char *buf, size_t *size)
{
	char			*arrows;
	char			*arrow_addr;
	static char		esc_seq = 0;
	static char		esc_bytes_checked = 0;

	arrows = "ABCD";
	if (!esc_seq && buf[*size - 1] == '\e')
	{
		esc_seq = 1;
		buf[*size - 1] = '\0';
		*size -= 1;
		return (1);
	}
	else if (esc_seq && !esc_bytes_checked)
	{
		if (buf[*size - 1] == '[')
			esc_bytes_checked++;
		else
		{
			esc_bytes_checked = 0;
			esc_seq = 0;
			buf[*size - 1] = '\0';
			*size -= 1;
		}
		return (1);
	}
	else if (esc_seq && esc_bytes_checked == 1)
	{
		arrow_addr = ft_strchr(arrows, buf[*size - 1]);
		if (arrow_addr)
		{
			if (arrow_addr < (arrows + 2))
				printf("BAILA MALDITO (CMD HIST)\n");
			else
				printf("NO PASHA NA TRONQUI (DESPLAZ FLECHA)\n");
			buf[*size - 1] = '\0'; //delete chars from buf
			buf[*size - 2] = '\0';
			*size -= 2; //decrement size counter
		}
		else
			write(STDIN_FILENO, &buf[*size - 2], 2);
		esc_bytes_checked = 0;
		esc_seq = 0;
		return (1);
	}
	return (0);
}


char	handle_esc_seq(char *main_buf, size_t *size)
{
	static char		sub_buf[2] = {0,0}; //resetea al quitar esc
	char			*arrows;
	char			*arrow_addr;
	static char		esc_bytes_checked = 0;

	arrows = "ABCD";
	if (!esc_bytes_checked)
	{
		sub_buf[0] = main_buf[*size - 1];
		esc_bytes_checked++;
	}
	else if (esc_bytes_checked == 1)
	{
		sub_buf[1] = main_buf[*size - 1];
		if (sub_buf[0] == '[')
		{
			arrow_addr = ft_strchr(arrows, sub_buf[1]);
			if (arrow_addr)
			{
				if (arrow_addr < (arrows + 2))
					printf("BAILA MALDITO (CMD HIST)\n");
				else
					printf("NO PASHA NA TRONQUI (DESPLAZ FLECHA)\n");
				main_buf[*size - 1] = '\0'; //delete chars from buf
				main_buf[*size - 2] = '\0';
				*size -= 2; //decrement size counter
			}
			else
				write(STDIN_FILENO, sub_buf, 2);
			esc_bytes_checked = 0;
			return (0);
		}
	}
	return (1);
}