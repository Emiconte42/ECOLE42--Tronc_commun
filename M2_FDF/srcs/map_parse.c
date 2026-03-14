/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_parse.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 19:11:50 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/18 12:51:44 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

/*****************************************************************************
							PARSE THE MAP FILE
			READ EACH LINE / CONVERT VALUES / FILL THE MAP GRID
*****************************************************************************/

/*======================================================
Extract altitude value from token
Ignore optional color part (z,0xHEX)
======================================================*/
static int	parse_z(char *token)
{
	char	*comma;

	comma = ft_strchr(token, ',');
	if (comma)
		*comma = '\0';
	return (ft_atoi(token));
}

/*======================================================
Remove trailing newline from line read by GNL
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
Split a line, convert values and store them in the map
======================================================*/
static int	parse_line(t_fdf *fdf, char *line, int y)
{
	char	**split;
	int		x;

	trim_newline(line);
	split = ft_split(line, ' ');
	if (!split)
		return (0);
	x = 0;
	while (split[x] && x < fdf->map.width)
	{
		fdf->map.data[y][x] = parse_z(split[x]);
		free(split[x]);
		x++;
	}
	if (x != fdf->map.width || split[x])
	{
		while (split[x])
			free(split[x++]);
		free(split);
		return (0);
	}
	free(split);
	return (1);
}

/*======================================================
Read each line from the file and parse it
======================================================*/
static int	parse_lines(t_fdf *fdf, int fd)
{
	int		y;
	char	*line;

	y = 0;
	while (y < fdf->map.height)
	{
		line = get_next_line(fd);
		if (!line)
			return (0);
		if (parse_line(fdf, line, y) == 0)
		{
			free(line);
			return (0);
		}
		free(line);
		y++;
	}
	return (1);
}

/*======================================================
Parse the map file and fill the grid
======================================================*/
int	map_parse(const char *filename, t_fdf *fdf)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		ft_putendl_fd("Error", 2);
		return (0);
	}
	if (parse_lines(fdf, fd) == 0)
	{
		close(fd);
		return (0);
	}
	close(fd);
	return (1);
}
