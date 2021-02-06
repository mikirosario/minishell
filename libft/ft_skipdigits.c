/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_skipdigits.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/21 20:13:11 by mrosario          #+#    #+#             */
/*   Updated: 2020/07/21 20:29:32 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_skipdigits(const char *line)
{
	if (line)
		while (*line && ft_isdigit(*line))
			line++;
	return ((char *)line);
}
