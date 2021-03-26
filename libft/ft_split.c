/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/11 03:22:29 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/26 00:00:44 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** Wrapper for ft_strtrim. Remove character passed as separator from either end
** of the string passed as s and return a copy of the result.
*/

static char	*trim(char const *s, char c)
{
	char	*scpy;
	char	set[2];

	set[0] = c;
	set[1] = '\0';
	scpy = (char *)s;
	scpy = ft_strtrim(scpy, set);
	if (!scpy)
		return (NULL);
	return (scpy);
}

/*
** Count the number of c-separated substrings in scpy and return the result.
** Since the string is trimmed of c characters at the ends, we know that if we
** find a non-NUL after ft_strchr(scpy, c), there MUST be at least one non-NUL
** non-c character after it, so we need to count a new substring.
**
** Since scpy does point to something if we enter this function, we check if it
** is just a NUL char. If that is the case, it doesn't count as a string, so
** then we have zero substrings, so we will just have the NULL pointer.
** Therefore, in that case we return 0, which we will add one to later for the
** NULL.
*/

static size_t	calculate_ptr_array_size(char *scpy, char c)
{
	size_t	substr_count;

	substr_count = 0;
	if (*scpy == '\0')
		return (0);
	while (scpy && *scpy)
	{
		substr_count++;
		while (*scpy == c)
			scpy++;
		scpy = ft_strchr(scpy, c);
	}
	return (substr_count);
}

/*
** This function is like normal strlen, except it will also stop at the closest
** delimiter character (c). This will thus return the number of bytes from the
** address to the next delimiter character, or the terminating null character if
** there are no delimiter characters.
*/

static size_t	split_strlen(char *substr, char c)
{
	size_t	i;

	i = 0;
	while (substr[i] && substr[i] != c)
		i++;
	return (i);
}

/*
** If the string is empty except for a null terminator, point the first pointer
** in the pointer array to NULL. Note that in this case, this will also be the
** ONLY pointer in the array, so we immediately return after that.
**
** Otherwise, we will iterate through the string, get the length of each
** substring using split_strlen, reserve enough memory for the sub-string plus
** a null termination, and use memcpy to copy the substring to the new memory.
** We then use ft_strchr to advance the scpy index to the next separator, or the
** null terminator.
**
** If at any point memory allocation fails, we return 0 for failure. Otherwise,
** we return 1 for success.
*/

static int	split_string(char *scpy, char **split, char c)
{
	size_t	i;
	size_t	strlen;

	if (*scpy == '\0')
		split[0] = NULL;
	else
	{
		i = 0;
		while (scpy && *scpy)
		{
			while (*scpy == c)
				scpy++;
			strlen = split_strlen(scpy, c);
			split[i] = ft_calloc(strlen + 1, sizeof(char));
			if (!split[i])
				return (0);
			ft_memcpy(split[i], scpy, strlen);
			scpy = ft_strchr(scpy, c);
			i++;
		}
	}
	return (1);
}

/*
** Confirm existence of s string and, if it exists, trim the separator off
** either end and save the address of the trimmed string at scpy.
**
** Return NULL if trim fails.
**
** Reserve an array with enough pointers to hold all of the c-separated
** sub-strings in the string, as calculated by calculate_ptr_array_size.
**
** If the reservation succeeds, split the string, and point each pointer in the
** array to a successive sub-string, except for the null pointer at the end. If
** split_string succeeds as well, free the memory occupied by scpy and return a
** double pointer to the split.
**
** If either the initial reservation or split_string (at any point) fails to
** reserve the memory needed for the split, free whichever part of the split has
** been partially reserved, if any, and free the memory occupied by scpy, then
** return NULL.
*/

char	**ft_split(char const *s, char c)
{
	char	*scpy;
	char	**split;

	scpy = trim(s, c);
	if (!scpy)
		return (NULL);
	split = ft_calloc(calculate_ptr_array_size(scpy, c) + 1, sizeof(char *));
	if (split && split_string(scpy, split, c))
	{
		scpy = ft_del(scpy);
		return (split);
	}
	if (split)
		ft_free_split(split);
	scpy = ft_del(scpy);
	return (NULL);
}
