/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   5_doors.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 11:49:42 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/17 13:25:05 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static int	ft_is_door_transparent(unsigned int color)
{
	int	r;
	int	g;
	int	b;

	r = (color >> 16) & 0xFF;
	g = (color >> 8) & 0xFF;
	b = color & 0xFF;
	return (r >= 45 && r <= 65 && g >= 45 && g <= 65 && b >= 45 && b <= 65);
}

static void	ft_dda_doors(t_game *game, t_ray *ray)
{
	ray->hit = 0;
	while (ray->hit == 0)
	{
		if (ray->side_dist_x < ray->side_dist_y)
		{
			ray->side_dist_x += ray->delta_dist_x;
			ray->map_x += ray->step_x;
			ray->side = 0;
		}
		else
		{
			ray->side_dist_y += ray->delta_dist_y;
			ray->map_y += ray->step_y;
			ray->side = 1;
		}
		if (game->map.grid[ray->map_y][ray->map_x] == '1'
			|| game->map.grid[ray->map_y][ray->map_x] == 'D')
			ray->hit = 1;
	}
}

static int	get_tex_y(double tex_pos, int height)
{
	int	tex_y;

	tex_y = (int)tex_pos;
	if (tex_y < 0)
		return (0);
	if (tex_y >= height)
		return (height - 1);
	return (tex_y);
}

static void	ft_draw_door_stripe(t_game *game, t_ray *ray, int x, t_img *tex)
{
	int				y;
	double			step;
	double			tex_pos;
	unsigned int	color;

	step = 1.0 * tex->height / ray->line_height;
	tex_pos = (ray->draw_start - (double)WINDOW_HEIGHT / 2
			+ (double)ray->line_height / 2) * step;
	y = ray->draw_start;
	while (y < ray->draw_end)
	{
		color = ft_get_pixel_from_tex(tex, ray->tex_x,
				get_tex_y(tex_pos, tex->height));
		tex_pos += step;
		if (ft_is_door_transparent(color))
		{
			y++;
			continue ;
		}
		if (ray->side == 1)
			color = (color >> 1) & 8355711;
		put_pixel(game, x, y++, color);
	}
}

void	ft_render_door(t_game *game, t_ray ray, int x, t_img *tex)
{
	t_ray	doors;
	t_img	*doors_tex;

	doors = ray;
	ft_dda_doors(game, &doors);
	ft_calculate_wall_dist(&doors);
	ft_calculate_wall_height(&doors);
	doors_tex = ft_get_tex(game, &doors);
	ft_calculate_tex_x(game, &doors, doors_tex);
	if (game->map.grid[doors.map_y][doors.map_x] == 'D')
		ft_render_door(game, doors, x, doors_tex);
	else
	{
		ft_draw_floor_ceiling(game, &doors, x);
		ft_draw_tex_stripe(game, &doors, x, doors_tex);
	}
	ft_draw_door_stripe(game, &ray, x, tex);
}
