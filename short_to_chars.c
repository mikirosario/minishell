/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   short_to_chars.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miki <miki@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/24 18:46:28 by miki              #+#    #+#             */
/*   Updated: 2021/03/24 22:27:18 by miki             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** This function joins the string of the second parameter to the string in the
** first parameter if the string in the first parameter exists. Otherwise, it
** just returns the string in the second parameter. This is different than
** ft_strjoin, which returns NULL if the first parameter does not exist.
**
** It doesn't null-check the second parameter as it is a bespoke function that
** is called behind a null-check (since the calling function does not want the
** char_str to be nullified if there is no short_char). You might want to
** include a null-check though if you use it in other contexts.
*/

char	*join_to_string(char *char_str, char *short_char)
{
	char	*delete;

	if (char_str)
	{
		delete = char_str;
		char_str = ft_strjoin(char_str, short_char);
		delete = ft_del(delete);
	}
	else
		char_str = ft_strdup(short_char);
	short_char = ft_del(short_char);
	return (char_str);
}

/*
** This function converts a short (two bytes) into a null-terminated string of
** up to two characters. Any insigificant bytes (zeroed high bytes) are
** discarded. This function is for LITTLE-ENDIAN systems only, so it is assumed
** that the least significant bytes are at the lowest memory addresses.
**
** The while thus iterates through the character buffer until a significant byte
** is found, at which point it breaks out of the loop.
**
** A copy of the character buffer from the first significant byte onwards is
** returned.
**
** If no bytes in the short were significant (the short was 0), then NULL is
** returned.
*/

char	*short_to_chars(short short_char)
{
	unsigned char	charbuf[3];
	char			*char_index;
	int				i;

	ft_bzero(charbuf, 3);
	i = 0;
	charbuf[0] = short_char & 0xff;
	charbuf[1] = (short_char >> 8) & 0xff;
	char_index = (char *)charbuf;
	while (i++ < 2)
	{
		if (!(*char_index))
			char_index++;
		else
			break ;
	}
	if (*char_index)
		return (ft_strdup(char_index));
	return (NULL);
}

/*
** This function accepts a null-terminated string of shorts and spits out a
** null_terminated string of chars, eliminating all the padding bytes, no
** questions asked. If the operation fails, NULL is returned.
**
** The function ft_strlen16 is used to measure the length of the short strong.
** Each character in the short string is then passed to the short_to_chars
** auxiliary function, which converts it into a character string of one or two
** chars, as needed to store the same value, called short_char.
**
** The join_to_string function is then called to concatenate short_char to the
** character string whose address will be returned to the user. This is
** reapeated until the entire short string is reassembled as a character string.
** If no prior character string exists yet, join_to_string will create a new
** character string with short_char as the stem.
**
** The completed character string is returned to the user. NOTE: Like normal dup
** functions, this function will not free the original string.
**
** WARNING: For little-endian systems only!
**
** EL PLAN BORRÓN!!!! Mwajajajaja. xD
*/

char	*ft_short_to_strdup(short *short_str)
{
	size_t	strlen16;
	size_t	i;
	char	*char_str;
	char	*short_char;

	char_str = NULL;
	if (!short_str)
		return (NULL);
	strlen16 = ft_strlen16(short_str);
	i = 0;
	while (i < strlen16)
	{
		short_char = short_to_chars((unsigned short)short_str[i]);
		if (short_char)
			char_str = join_to_string(char_str, short_char);
		i++;
	}
	return (char_str);
}
