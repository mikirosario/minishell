/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   iamerror.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/08 19:25:04 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/15 11:50:15 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_error(char *error_message, char *error_location)
{
	ft_printf("%s '%.2s'\n", error_message, error_location); //Print two chars here but for echo ;| ls, for example, should only print '|'
}