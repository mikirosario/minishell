/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/23 12:57:58 by mrosario          #+#    #+#             */
/*   Updated: 2020/01/23 17:26:36 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

char	*ft_intchar(char *str, int endpos, unsigned long long int num, int base)
{
	str[endpos--] = '\0';
	while (endpos >= 0 && num > 0)
	{
		str[endpos--] = num % base < 10 ? (num % base) + 48 : (num % base) + 55;
		num = num / base;
	}
	return (str);
}

char	*ft_itoa_base(long long int num, int base)
{
	int						bytes;
	unsigned long long int	ncpy;
	char					*ptr;

	if (base < 2 || base > 36)
		return (ptr = ft_strdup("ft_itoa_base supports base values 2-32"));
	if (num == 0)
	{
		if (!(ptr = malloc(2 * sizeof(char))))
			return (ptr);
		ptr[1] = '\0';
		ptr[0] = '0';
		return (ptr);
	}
	bytes = 1;
	ncpy = num < 0 ? num * -1 : num;
	while ((ncpy = ncpy / base) > 0)
		bytes++;
	ncpy = num < 0 ? num * -1 : num;
	bytes = num < 0 ? bytes + 2 : bytes + 1;
	if (!(ptr = malloc(bytes * sizeof(char))))
		return (ptr);
	if (num < 0)
		*ptr = '-';
	return (ft_intchar(ptr, bytes - 1, ncpy, base));
}
