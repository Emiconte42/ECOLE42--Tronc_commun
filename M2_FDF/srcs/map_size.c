/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_size.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 17:13:22 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/16 10:52:47 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

/******************************************************************************
						DETERMINE MAP DIMENSIONS
				WIDTH  = NUMBER OF VALUES ON THE FIRST LINE
				HEIGHT = NUMBER OF LINES IN THE FILE
******************************************************************************/

/*======================================================
Ignore the \n in the map mars.fdf
======================================================*/
static void	trim_newline(char *s)
{
	int	len;

	if (!s)
		return ;
	len = ft_strlen(s);
	if (len > 0 && s[len - 1] == '\n')
		s[len - 1] = '\0';
}

/*======================================================
Count the number of values on a single line
======================================================*/
static int	count_width(char *line)
{
	char	**split;
	int		count;
	int		i;

	trim_newline(line);
	split = ft_split(line, ' ');
	if (!split)
		return (0);
	count = 0;
	while (split[count] != NULL)
		count++;
	i = 0;
	while (split[i])
		free(split[i++]);
	free(split);
	return (count);
}

/*===========================================================
Read file to determine map dimensions (width and height)
============================================================*/
static int	read_lines(int fd, int *width, int *height)
{
	char	*line;
	int		w;

	line = get_next_line(fd);
	while (line)
	{
		w = count_width(line);
		if (*height == 0)
			*width = w;
		else if (w != *width)
		{
			ft_putendl_fd("Error", 2);
			return (free(line), 0);
		}
		(*height)++;
		free(line);
		line = get_next_line(fd);
	}
	return (1);
}

/*============================================================
Compute map width and height from file
============================================================*/
int	map_size(const char *filename, int *width, int *height)
{
	int	fd;

	if (!width || !height)
		return (0);
	*width = 0;
	*height = 0;
	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		ft_putendl_fd("Error", 2);
		return (0);
	}
	if (!read_lines(fd, width, height))
		return (close(fd), 0);
	close(fd);
	if (*width == 0 || *height == 0)
	{
		ft_putendl_fd("Error", 2);
		return (0);
	}
	return (1);
}
