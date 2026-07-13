/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_dda.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 15:22:24 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/08 11:29:26 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static void	ft_dda_utils(t_game *game, t_ray *ray)
{
	t_door	*door;

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
	if (game->map.grid[ray->map_y][ray->map_x] == '1')
		ray->hit = 1;
	else if (game->map.grid[ray->map_y][ray->map_x] == 'D')
	{
		door = ft_get_door_at(game, ray->map_x, ray->map_y);
		if (door && door->frame < DOOR_ANIM_FRAMES - 1)
			ray->hit = 1;
	}
}

void	ft_dda(t_game *game, t_ray *ray)
{
	ray->hit = 0;
	while (ray->hit == 0)
		ft_dda_utils(game, ray);
}

void	ft_calculate_wall_dist(t_ray *ray)
{
	if (ray->side == 0)
		ray->perp_wall_dist = ray->side_dist_x - ray->delta_dist_x;
	else
		ray->perp_wall_dist = ray->side_dist_y - ray->delta_dist_y;
}

void	ft_calculate_wall_height(t_ray *ray)
{
	if (ray->perp_wall_dist == 0)
		ray->perp_wall_dist = 0.1;
	ray->line_height = (int)(WINDOW_HEIGHT / ray->perp_wall_dist);
	ray->draw_start = -ray->line_height / 2 + WINDOW_HEIGHT / 2;
	if (ray->draw_start < 0)
		ray->draw_start = 0;
	ray->draw_end = ray->line_height / 2 + WINDOW_HEIGHT / 2;
	if (ray->draw_end >= WINDOW_HEIGHT)
		ray->draw_end = WINDOW_HEIGHT - 1;
}
