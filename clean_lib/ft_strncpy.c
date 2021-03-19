/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/19 00:15:10 by miki              #+#    #+#             */
/*   Updated: 2021/03/19 21:10:25 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
** This function copies at most n bytes of src to dst. If less than n bytes are
** copied to dst, any remaining bytes are nul-padded. If there is no null byte
** among the first n bytes of src, the string will not be null-terminated. If n
** is greater than the size of dst, a buffer overrun will be caused.
*/

char	*ft_strncpy(char *dst, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	if (n != 0)
	{
		while (src[i] && i < n)
		{
			dst[i] = src[i];
			i++;
		}
		while (i < n)
		{
			dst[i] = '\0';
			i++;
		}
	}
	return (dst);
}
