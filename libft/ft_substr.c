/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/09 17:56:50 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/25 03:49:52 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char const	*substr;
	char		*substrcpy;
	size_t		substrsize;

	if (!s)
		return (0);
	if (((ft_strlen(s)) - 1) < start)
	{
		substrcpy = ft_calloc(1, 1);
		return (substrcpy);
	}
	substr = &s[start];
	substrsize = (len + 1);
	substrcpy = ft_calloc(substrsize, 1);
	if (!substrcpy)
		return (substrcpy);
	substrcpy = (ft_memcpy(substrcpy, substr, len));
	return (substrcpy);
}
