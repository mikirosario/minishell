/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isbitset.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/26 20:42:08 by mrosario          #+#    #+#             */
/*   Updated: 2021/04/01 20:03:43 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** Simple function to tell you whether a given bit is set in a byte.
**
** Bit 0 is the rightmost bit. Returns !0 if set, 0 if not set.
**
** If the bitshift is set higher than 8, behaviour is undefined. Don't do it. ;)
*/

unsigned char	ft_isbitset(unsigned char byte, unsigned char bit)
{
	unsigned char	mask;

	mask = (unsigned char) 1 << bit;
	return (mask & byte);
}
