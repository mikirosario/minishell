/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/12 18:44:01 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/26 02:06:11 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*zero(void)
{
	char	*ptr;

	ptr = ft_calloc(2, sizeof(char));
	if (ptr)
		*ptr = '0';
	return (ptr);
}

/*
** Recurisve function. The positive number passed as a parameter is divided by
** 10. If the quotient is greater than 0, it returns the result of a further
** execution of itself with the new value of ncpy + 1. When the quotient is not
** greater than 0, 1 is returned, which will be added to 1 in the previous
** function call, giving 2, and that will likewise be added to 1 recursively.
** Each division represents one digit in the number. The end result will be the
** number of digits in the number.
**
** Got bored doing things iteratively, and I have to rewrite this for the new
** norminette, so I'm practising recursion again. :P
*/

static size_t	count_digits(long long int ncpy)
{
	ncpy = ncpy / 10;
	if (ncpy > 0)
		return (count_digits(ncpy) + 1);
	return (1);
}

static char	*int_to_char(char *ptr, size_t endpos, long long int ncpy)
{
	while (ncpy > 0)
	{
		ptr[--endpos] = (ncpy % 10) + 48;
		ncpy = ncpy / 10;
	}
	return (ptr);
}

char	*ft_itoa(int n)
{
	long long int	ncpy;
	size_t			bytes;
	char			*ptr;

	if (n == 0)
		return (zero());
	ncpy = (long long int)n;
	if (ncpy < 0)
		ncpy *= -1;
	bytes = count_digits(ncpy);
	if (n < 0)
		bytes += 2;
	else
		bytes += 1;
	ptr = ft_calloc(bytes, sizeof(char));
	if (!ptr)
		return (ptr);
	if (n < 0)
		*ptr = '-';
	return (int_to_char(ptr, bytes - 1, ncpy));
}
