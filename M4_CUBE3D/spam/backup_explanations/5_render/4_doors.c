/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   4_doors.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 11:49:42 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/06 12:52:57 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

int	ft_is_door_transparent(unsigned int color)
{
	int	r;
	int	g;
	int	b;

	r = (color >> 16) & 0xFF;
	g = (color >> 8) & 0xFF;
	b = color & 0xFF;
	return (r >= 45 && r <= 65 && g >= 45 && g <= 65 && b >= 45 && b <= 65);
}

void	ft_dda_bg(t_game *game, t_ray *ray)
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

void	ft_draw_door_stripe(t_game *game, t_ray *ray, int x, t_img *tex)
{
	int				y;
	double			step;
	double			tex_pos;
	int				tex_y;
	unsigned int	color;

	step = 1.0 * tex->height / ray->line_height;
	tex_pos = (ray->draw_start - (double)WINDOW_HEIGHT / 2
			+ (double)ray->line_height / 2) * step;
	y = ray->draw_start;
	while (y < ray->draw_end)
	{
		tex_y = (int)tex_pos;
		if (tex_y >= tex->height)
			tex_y = tex->height - 1;
		else if (tex_y < 0)
			tex_y = 0;
		tex_pos += step;
		color = ft_get_pixel_from_tex(tex, ray->tex_x, tex_y);
		if (ft_is_door_transparent(color))
		{
			y++;
			continue ;
		}
		if (ray->side == 1)
			color = (color >> 1) & 8355711;
		put_pixel(game, x, y, color);
		y++;
	}
}

void	ft_render_door(t_game *game, t_ray ray, int x, t_img *tex)
{
	t_ray	bg;
	t_img	*bg_tex;

	bg = ray;
	ft_dda_bg(game, &bg);
	ft_calculate_wall_dist(&bg);
	ft_calculate_wall_height(&bg);
	bg_tex = ft_get_tex(game, &bg);
	ft_calculate_tex_x(game, &bg, bg_tex);
	if (game->map.grid[bg.map_y][bg.map_x] == 'D')
		ft_render_door(game, bg, x, bg_tex);
	else
		ft_draw_tex_stripe(game, &bg, x, bg_tex);
	ft_draw_door_stripe(game, &ray, x, tex);
}
