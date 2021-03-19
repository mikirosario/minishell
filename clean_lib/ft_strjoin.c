/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/09 19:37:02 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/19 21:10:25 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	s1size;
	size_t	s2size;
	char	*combo;

	if (!s1 || !s2)
		return (0);
	s1size = (ft_strlen(s1)) + 1;
	s2size = (ft_strlen(s2)) + 1;
	if (!(combo = (char *)ft_calloc(s1size - 1 + s2size, 1)))
		return (combo);
	ft_strlcpy(combo, s1, s1size);
	ft_strlcat(combo, s2, s1size + s2size);
	return (combo);
}
