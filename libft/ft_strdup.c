/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/09 16:35:18 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/25 03:43:18 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(char const *s1)
{
	size_t	strsize;
	char	*ptr;

	strsize = (ft_strlen(s1)) + 1;
	ptr = ft_calloc(strsize, 1);
	if (!ptr)
		return (ptr);
	ft_memcpy(ptr, s1, strsize);
	return (ptr);
}
