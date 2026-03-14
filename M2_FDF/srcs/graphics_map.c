/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   graphics_map.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 12:14:05 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/15 18:53:16 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

/*****************************************************************************
					DRAW THE ISOMETRIC WIREFRAME MAP
*****************************************************************************/

/*===========================================
Convert 3D grid coordinates (x, y, z) 
into 2D isometric screen coordinates.
===========================================*/
static t_point	iso_project(t_fdf *fdf, int x, int y, int z)
{
	t_point	p;

	p.x = (x - y) * fdf->view.spacing + fdf->view.origin_x;
	p.y = (x + y) * fdf->view.spacing / 2
		+ fdf->view.origin_y - (z * Z_SCALE);
	return (p);
}

/*===========================================
Draw horizontal and vertical links
between neighboring projected points.
===========================================*/
static void	draw_links(t_fdf *fdf, t_point p, int x, int y)
{
	t_point	point_right;
	t_point	point_down;
	int		color;

	if (x + 1 < fdf->map.width)
	{
		point_right = iso_project(fdf, x + 1, y,
				fdf->map.data[y][x + 1]);
		color = get_color((fdf->map.data[y][x] + fdf->map.data[y][x + 1]) / 2,
				fdf);
		draw_line(fdf, p, point_right, color);
	}
	if (y + 1 < fdf->map.height)
	{
		point_down = iso_project(fdf, x, y + 1,
				fdf->map.data[y + 1][x]);
		color = get_color((fdf->map.data[y][x] + fdf->map.data[y + 1][x]) / 2,
				fdf);
		draw_line(fdf, p, point_down, color);
	}
}

/*===========================================
Iterate through the map grid,
project each point and draw its connections.
===========================================*/
void	draw_map(t_fdf *fdf)
{
	int		x;
	int		y;
	t_point	p;

	y = 0;
	while (y < fdf->map.height)
	{
		x = 0;
		while (x < fdf->map.width)
		{
			p = iso_project(fdf, x, y,
					fdf->map.data[y][x]);
			draw_links(fdf, p, x, y);
			x++;
		}
		y++;
	}
}
