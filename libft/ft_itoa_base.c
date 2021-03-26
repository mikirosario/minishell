/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/23 12:57:58 by mrosario          #+#    #+#             */
/*   Updated: 2021/03/26 02:09:43 by miki             ###   ########.fr       */
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

static size_t	count_digits(long long int ncpy, int base)
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
** This function converts the number passed as in to the base passed as base
** and returns a pointer to a character string representing that number. All the
** bases start from 0 and have additional values represented first by 1-9 and
** then A-Z, so they are really just extended-hex bases.
**
** Except for base 10, negative numbers are ignored.
**
** If an unsupported base is used, an error message is returned instead of the
** number. This message must also be freed.
**
** If memory allocation fails at any point, NULL is returned.
*/

char	*ft_itoa_base(long long int n, int base)
{
	unsigned long long int	ncpy;
	char					*ptr;
	int						bytes;

	if (base < 2 || base > 36)
		return (ft_strdup("ft_itoa_base supports base values 2-36"));
	if (n == 0)
		return (zero());
	ncpy = (long long int)n;
	if (n < 0)
		ncpy *= -1;
	bytes = count_digits(ncpy, base);
	if (n < 0 && base == 10)
		bytes += 2;
	else
		bytes += 1;
	ptr = ft_calloc(bytes, sizeof(char));
	if (!ptr)
		return (ptr);
	if (n < 0 && base == 10)
		*ptr = '-';
	return (int_to_char(ptr, bytes - 1, ncpy, base));
}
