/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free_split.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/19 03:20:45 by miki              #+#    #+#             */
/*   Updated: 2021/03/19 03:23:39 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** Frees all memory reserved for a character pointer array with ft_split, first
** freeing the lines pointed to by each pointer, then freeing the pointer array
** itself.
*/

char	**ft_free_split(char **split)
{
	int	i;

	i = 0;
	while (split[i])
	{
		split[i] = ft_del(split[i]);
		i++;
	}
	free(split);
	return (NULL);
}
