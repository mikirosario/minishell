/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/11 03:22:29 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/19 18:38:49 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ptr_array_len(char const *s, char c)
{
	size_t	len;

	len = 0;
	if (*s)
	{
		while (*s)
		{
			if (*s == c && *(s - 1) != c)
				len++;
			s++;
		}
		if (*(s - 1) != c)
			len++;
	}
	return (len);
}

static size_t	str_fragment_len(char const *s, char c)
{
	size_t	strlen;

	strlen = 0;
	while (s[strlen] && s[strlen] != c)
		strlen++;
	return (strlen);
}

static char		**free_split(char **split, size_t y, char *strim)
{
	size_t	i;

	if (strim)
		free(strim);
	if (y)
	{
		i = 0;
		while (i < y)
		{
			free(split[i]);
			split[i] = NULL;
			i++;
		}
		free(split);
		split = NULL;
	}
	return (split);
}

static char		**split_str_cpy(char **split, size_t array_len, char *strim,
								char c)
{
	char	*s;
	size_t	y;
	size_t	x;

	s = strim;
	y = 0;
	while (array_len--)
	{
		if (!(split[y] = ft_calloc((str_fragment_len(s, c) + 1), sizeof(char))))
			return (free_split(split, y, strim));
		x = 0;
		while (*s && *s != c)
			split[y][x++] = *s++;
		while (*s && *s == c)
			s++;
		y++;
	}
	return (free_split(split, 0, strim));
}

char			**ft_split(char const *s, char c)
{
	char	**split;
	char	*strim;
	size_t	array_len;

	strim = NULL;
	if (!s || !(strim = ft_strtrim(s, (char[2]) {c, '\0'})) || !*strim)
		return (free_split(NULL, 0, strim));
	array_len = ptr_array_len(s, c);
	if (!(split = ft_calloc(array_len + 1, sizeof(char *))))
		return (free_split(split, 0, strim));
	split[array_len] = NULL;
	return (split_str_cpy(split, array_len, strim, c));
}
