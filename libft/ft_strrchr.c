/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/06 22:44:12 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/25 03:47:19 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(char const *s, int c)
{
	char	ch;
	char	*lastval;

	ch = c;
	lastval = 0;
	while (*s != '\0')
	{
		if (*s == ch)
			lastval = (char *)s;
		s++;
	}
	if (*s == ch)
		lastval = (char *)s;
	return (lastval);
}
