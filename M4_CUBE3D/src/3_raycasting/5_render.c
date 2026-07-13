/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   5_render.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 15:22:33 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/08 11:30:24 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static void	ft_cast_rays_utils(t_game *game, t_ray *ray, int x)
{
	ray->camera_x = 2 * x / (double)WINDOW_WIDTH - 1;
	ray->dir_x = game->player.dir_x + game->player.plane_x * ray->camera_x;
	ray->dir_y = game->player.dir_y + game->player.plane_y * ray->camera_x;
	ray->map_x = (int)game->player.pos_x;
	ray->map_y = (int)game->player.pos_y;
	if (ray->dir_x == 0)
		ray->delta_dist_x = 1e30;
	else
		ray->delta_dist_x = fabs(1.0 / ray->dir_x);
	if (ray->dir_y == 0)
		ray->delta_dist_y = 1e30;
	else
		ray->delta_dist_y = fabs(1.0 / ray->dir_y);
}

void	ft_cast_rays(t_game *game)
{
	int		x;
	t_img	*tex;
	t_ray	ray;

	x = 0;
	while (x < WINDOW_WIDTH)
	{
		ft_cast_rays_utils(game, &ray, x);
		ft_init_ray(game, &ray);
		ft_dda(game, &ray);
		ft_calculate_wall_dist(&ray);
		ft_calculate_wall_height(&ray);
		tex = ft_get_tex(game, &ray);
		ft_calculate_tex_x(game, &ray, tex);
		if (game->map.grid[ray.map_y][ray.map_x] == 'D')
			ft_render_door(game, ray, x, tex);
		else
			ft_draw_tex_stripe(game, &ray, x, tex);
		x++;
	}
}
