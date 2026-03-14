/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_manager.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 11:32:04 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/18 17:27:30 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

/*****************************************************************************
							MANAGE MAP LIFECYCLE 
			SIZE DETECTION / ALLOCATION / PARSING / DESTRUCTION
*****************************************************************************/

/*======================================================
Allocate memory for the map grid
======================================================*/
static int	**map_alloc(int width, int height)
{
	int		**data;
	int		y;

	y = 0;
	data = malloc(sizeof(int *) * height);
	if (!data)
		return (NULL);
	while (y < height)
	{
		data[y] = malloc(sizeof(int) * width);
		if (!data[y])
		{
			while (y > 0)
			{
				y--;
				free(data[y]);
			}
			free(data);
			return (NULL);
		}
		y++;
	}
	return (data);
}

/*======================================================
Load the map:
- get map size
- allocate memory
- parse values 
======================================================*/
int	map_load(t_fdf *fdf, const char *filename)
{
	int		width;
	int		height;

	if (map_size(filename, &width, &height) == 0)
		return (0);
	(void)filename;
	if (width <= 0 || height <= 0)
		return (0);
	fdf->map.width = width;
	fdf->map.height = height;
	fdf->map.filename = (char *)filename;
	fdf->map.data = map_alloc(width, height);
	if (!fdf->map.data)
		return (0);
	if (map_parse(filename, fdf) == 0)
	{
		map_free(fdf);
		return (0);
	}
	find_min_max(fdf);
	return (1);
}

/*======================================================
Free all memory used by the map
======================================================*/
void	map_free(t_fdf *fdf)
{
	int	y;

	if (!fdf || !fdf->map.data)
		return ;
	y = 0;
	while (y < fdf->map.height)
	{
		free(fdf->map.data[y]);
		y++;
	}
	free(fdf->map.data);
	fdf->map.data = NULL;
}
