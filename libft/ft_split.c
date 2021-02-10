/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/11 03:22:29 by mrosario          #+#    #+#             */
/*   Updated: 2021/02/10 20:22:17 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

/*
** Miki's split had a leak, so we have substituted it for Miguel's split. xD
*/

int		ft_contador(char const *s, char c)
{
	unsigned int	i;
	int				cntr;
	int				tiene;

	i = 0;
	cntr = 0;
	tiene = 0;
	while (s[i] == c && s[i])
		i++;
	while (s[i])
	{
		if (s[i] != c && s[i])
			tiene = 1;
		if (s[i] == c)
		{
			while (s[i] == c && s[i])
				i++;
			if (s[i])
				cntr++;
		}
		else
			i++;
	}
	return (cntr + tiene);
}

char	*ft_alc(const char *s, char c)
{
	char	*tb;
	int		i;

	i = 0;
	tb = 0;
	while (s[i] && s[i] != c)
		i++;
	if (!(tb = malloc(i + 1)))
		return (NULL);
	ft_strlcpy(tb, s, i + 1);
	return (tb);
}

char	**ft_split(char const *s, char c)
{
	int		count;
	int		words;
	char	**tab;

	count = -1;
	if (!s)
		return (NULL);
	words = ft_contador(s, c);
	if (!(tab = malloc(sizeof(char *) * (words + 1))))
		return (NULL);
	while (++count < words)
	{
		while (s[0] == c)
			s++;
		if (!(tab[count] = ft_alc(s, c)))
		{
			while (count > 0)
				free(tab[count--]);
			free(tab);
			return (NULL);
		}
		s += ft_strlen(tab[count]);
	}
	tab[count] = NULL;
	return (tab);
}
