/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_chrcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/03 20:13:45 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/25 01:32:39 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Searches for char in str. If char is found, returns index of char.
** If char is not found, returns -1. For finding newlines.
*/

int	ft_chrcmp(char *s1, char c)
{
	int	i;

	i = 0;
	if (!s1)
		return (-1);
	while (s1[i])
	{
		if (s1[i] == c)
			return (i);
		i++;
	}
	return (-1);
}
