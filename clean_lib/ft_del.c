/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_del.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 20:46:19 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/19 21:10:25 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
** This function is a generic, run-of-the-mill "free this memory and set the
** pointer to NULL" function. :p
*/

void	*ft_del(void *freethis)
{
	free(freethis);
	return (NULL);
}
