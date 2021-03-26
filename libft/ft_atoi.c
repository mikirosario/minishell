/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/08 22:07:54 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/25 01:42:14 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** This used to be the work of a ternary, until they were banned by 42 school.
** Need I say more? Need I roll my eyes any further back into my head?
*/

static int	norminette_made_me_do_it(int sign)
{
	if (sign == 1)
		return (-1);
	else
		return (0);
}

int	ft_atoi(char const *str)
{
	char const			*ptr;
	int					base;
	long unsigned int	res;
	int					sign;

	ptr = str;
	base = 10;
	sign = 1;
	res = 0;
	while (*ptr == '\t' || *ptr == '\n' || *ptr == '\v'
		|| *ptr == '\f' || *ptr == '\r' || *ptr == ' ')
		ptr++;
	if (*ptr == '-')
	{
		ptr++;
		sign = -1;
	}
	else if (*ptr == '+')
		ptr++;
	while (ft_isdigit(*ptr))
		res = (res * base) + (*ptr++ - 48);
	if (res < 2147483649)
		return ((int)res * sign);
	return (norminette_made_me_do_it(sign));
}
