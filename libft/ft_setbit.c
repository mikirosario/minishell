/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_setbit.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 20:40:29 by mrosario          #+#    #+#             */
/*   Updated: 2021/04/01 20:03:23 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** Function to set a bit to 1. Returns bit-set variable.
**
** 0 is the rightmost bit.
**
** If bit is set higher than 8, behaviour is undefined.
*/

unsigned char	ft_setbit(unsigned char byte, unsigned char bit)
{
	unsigned char	mask;

	mask = (unsigned char) 1 << bit;
	return (mask | byte);
}
