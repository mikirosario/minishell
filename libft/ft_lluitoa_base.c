/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lluitoa_base.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/22 07:37:47 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/26 02:00:42 by miki             ###   ########.fr       */
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

static size_t	count_digits(unsigned long long int ncpy, int base)
{
	ncpy = ncpy / base;
	if (ncpy > 0)
		return (count_digits(ncpy, base) + 1);
	return (1);
}

static char	*int_to_char(char *str, int endpos, unsigned long long int ncpy, \
int base)
{
	unsigned long long int	modulo;

	while (ncpy > 0)
	{
		modulo = ncpy % base;
		if (modulo < 10)
			str[--endpos] = modulo + 48;
		else
			str[--endpos] = modulo + 55;
		ncpy = ncpy / base;
	}
	return (str);
}

/*
** This function is the same as ft_itoa_base, but can accomodate larger numbers
** as it takes unsigned long long ints. All numbers are treated as unsigned, so
** -1 == ULONG_MAX (18446744073709551615). Any number longer than ULONG_MAX will
** cause an overflow.
**
** This function converts the number passed as in to the base passed as base
** and returns a pointer to a character string representing that number. All the
** bases start from 0 and have additional values represented first by 1-9 and
** then A-Z, so they are really just extended-hex bases.
**
** If an unsupported base is used, an error message is returned instead of the
** number. This message must also be freed.
**
** If memory allocation fails at any point, NULL is returned.
*/

char	*ft_lluitoa_base(unsigned long long int n, int base)
{
	unsigned long long int	ncpy;
	char					*ptr;
	int						bytes;

	if (base < 2 || base > 36)
		return (ft_strdup("ft_itoa_base supports base values 2-36"));
	if (n == 0)
		return (zero());
	ncpy = (unsigned long long int)n;
	bytes = count_digits(ncpy, base);
	bytes += 1;
	ptr = ft_calloc(bytes, sizeof(char));
	if (!ptr)
		return (ptr);
	return (int_to_char(ptr, bytes - 1, ncpy, base));
}
