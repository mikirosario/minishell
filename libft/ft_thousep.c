/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_thousep.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 19:42:54 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/25 04:29:41 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	norminette_made_me_do_it(char *sepnum, char *numstr, int strlen, \
char sep)
{
	int	comi;

	comi = 0;
	while (strlen >= 0 && numstr[strlen] != '-')
	{
		if (comi == 3)
		{
			*sepnum-- = sep;
			comi -= 3;
		}
		else
		{
			*sepnum-- = numstr[strlen--];
			comi++;
		}
	}
}

static int	ft_sepcount(char *numstr, int strlen)
{
	int		comcount;
	int		newlen;

	comcount = 0;
	if (numstr[0] != '-' && strlen > 3)
		comcount = 1 + ((strlen - 4) / 3);
	else if (numstr[0] == '-' && strlen > 4)
		comcount = 1 + ((strlen - 5) / 3);
	newlen = comcount + strlen;
	return (newlen);
}

char	*ft_thousep(char *numstr, char sep)
{
	int		strlen;
	int		newlen;
	char	*sepnum;

	if (!numstr || (!(ft_isprint(sep))))
		return (NULL);
	strlen = ft_strlen(numstr);
	newlen = ft_sepcount(numstr, strlen--);
	sepnum = ft_calloc(1 + newlen--, sizeof(char));
	if (!sepnum)
		return (NULL);
	if (numstr[0] == '-')
	{
		sepnum[0] = numstr[0];
		strlen--;
		numstr++;
	}
	norminette_made_me_do_it(&sepnum[newlen], numstr, strlen, sep);
	return (sepnum);
}
