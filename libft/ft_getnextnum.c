/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_getnextnum.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 20:50:15 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/25 01:55:58 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** This is a little function I've found very useful. It finds the next number
** from the position in a string passed as an argument.
**
** If your pointer is pointing to a number in a string, it will go through the
** number until the end and then go through any non-numerical characters after
** the number until it finds the next number or a NULL. If you're pointing to a
** part of the string that isn't a number, it will of course ignore the first
** while and directly go to the second one to find the next number.
**
** If it finds another number, it returns a pointer to it, otherwise it returns
** a NULL pointer. Great line saver. ^_^
**
** This is going in my libft.
*/

char	*ft_getnextnum(char *num)
{
	while (*num && ft_isdigit(*num))
		num++;
	while (*num && !(ft_isdigit(*num)))
		num++;
	if (*num)
		return (num);
	return (NULL);
}
