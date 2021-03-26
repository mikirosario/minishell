/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/05 22:19:56 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/25 03:06:42 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(void const *s1, void const *s2, size_t n)
{
	unsigned char const	*ptr1;
	unsigned char const	*ptr2;

	ptr1 = s1;
	ptr2 = s2;
	if (n > 0)
	{
		while (n-- > 0)
		{
			if (*ptr1++ != *ptr2++)
				return (*--ptr1 - *--ptr2);
		}
	}
	return (0);
}
