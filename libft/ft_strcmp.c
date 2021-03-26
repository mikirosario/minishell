/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/28 19:40:43 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/25 03:42:49 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2)
	{
		if (!(*s1))
			return (0);
		s1++;
		s2++;
	}
	if (*(unsigned char *)s1 < *(unsigned char *)s2)
		return (-1);
	return (1);
}
