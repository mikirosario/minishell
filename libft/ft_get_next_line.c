/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_get_next_line.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrosario <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/03 20:17:10 by mrosario          #+#    #+#             */
/*   Updated: 2020/02/03 20:37:26 by mrosario         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libft.h"

#define BUFFER_SIZE 4

static int	ft_ncases(char **qfd, char **line)
{
	char	*eraser;
	int		nlpos;

	nlpos = ft_chrcmp(*qfd, '\n');
	if (nlpos == -1)
	{
		free(*line);
		*line = ft_strdup(qfd[0]);
		free(*qfd);
		*qfd = NULL;
		return (0);
	}
	else
	{
		eraser = *qfd;
		qfd[0][nlpos] = '\0';
		free(*line);
		*line = ft_strdup(qfd[0]);
		if (qfd[0][nlpos + 1])
			*qfd = ft_strdup(&qfd[0][nlpos + 1]);
		else
			*qfd = NULL;
		free(eraser);
	}
	return (1);
}

static int	ft_result(char **qfd, char **line, int ret)
{
	if (ret == -1)
		return (-1);
	else if (ret == 0 && !(*qfd))
		return (0);
	else
		return (ft_ncases(qfd, line));
}

int			ft_get_next_line(int fd, char **line)
{
	static char	*queue[4096];
	char		buf[BUFFER_SIZE + 1];
	char		*eraser;
	int			ret;

	if (!line || BUFFER_SIZE < 1 || fd < 0)
		return (-1);
	*line = ft_strdup("");
	if (queue[fd] && (ft_chrcmp(queue[fd], '\n') != -1))
		return (ft_ncases(&queue[fd], line));
	while ((ret = read(fd, buf, BUFFER_SIZE)) > 0)
	{
		buf[ret] = '\0';
		if (!(queue[fd]))
			queue[fd] = ft_strdup(buf);
		else
		{
			eraser = queue[fd];
			queue[fd] = ft_strjoin(queue[fd], buf);
			free(eraser);
		}
		if ((ft_chrcmp(buf, '\n') != -1))
			break ;
	}
	return (ft_result(&queue[fd], line, ret));
}
