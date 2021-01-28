/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_chrcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/03 20:13:45 by mrosario          #+#    #+#             */
/*   Updated: 2020/02/03 20:14:14 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Searches for char in str. If char is found, returns index of char.
** If char is not found, returns -1. For finding newlines.
*/

int		ft_chrcmp(char *s1, char c)
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
