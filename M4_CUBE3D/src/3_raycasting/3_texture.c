/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   3_texture.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 15:22:27 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/08 11:03:11 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

unsigned int	ft_get_pixel_from_tex(t_img *tex, int x, int y)
{
	int	*buffer;

	buffer = (int *)tex->img_addr;
	return (buffer[y * (tex->size_line / 4) + x]);
}

void	ft_calculate_tex_x(t_game *game, t_ray *ray, t_img *tex)
{
	if (ray->side == 0)
		ray->wall_x = game->player.pos_y + ray->perp_wall_dist * ray->dir_y;
	else
		ray->wall_x = game->player.pos_x + ray->perp_wall_dist * ray->dir_x;
	ray->wall_x -= floor(ray->wall_x);
	ray->tex_x = (int)(ray->wall_x * (double)tex->width);
	if (ray->side == 0 && ray->dir_x < 0)
		ray->tex_x = tex->width - ray->tex_x - 1;
	if (ray->side == 1 && ray->dir_y > 0)
		ray->tex_x = tex->width - ray->tex_x - 1;
}

t_img	*ft_get_tex(t_game *game, t_ray *ray)
{
	t_door	*door;

	if (game->map.grid[ray->map_y][ray->map_x] == 'D')
	{
		door = ft_get_door_at(game, ray->map_x, ray->map_y);
		if (door)
			return (&game->tex.door[door->frame]);
	}
	if (ray->side == 0)
	{
		if (ray->dir_x > 0)
			return (&game->tex.we);
		else
			return (&game->tex.ea);
	}
	else
	{
		if (ray->dir_y > 0)
			return (&game->tex.so);
		else
			return (&game->tex.no);
	}
}

void	ft_draw_tex_stripe(t_game *game, t_ray *ray, int x, t_img *tex)
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
		if (ray->side == 1)
			color = (color >> 1) & 8355711;
		put_pixel(game, x, y, color);
		y++;
	}
}
