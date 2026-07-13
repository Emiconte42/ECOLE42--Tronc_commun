/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   4_minimap_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 11:41:03 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/07 14:55:11 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

void	draw_map_border(t_game *game)
{
	int	left;
	int	right;
	int	top;
	int	bottom;

	left = game->minimap.origin_x;
	top = game->minimap.origin_y;
	right = left + game->map.width * game->minimap.tile_size;
	bottom = top + game->map.height * game->minimap.tile_size;
	game->minimap.border_x = left;
	while (game->minimap.border_x <= right)
	{
		put_pixel(game, game->minimap.border_x, top, 0x47ECFF);
		put_pixel(game, game->minimap.border_x, bottom, 0x47ECFF);
		game->minimap.border_x++;
	}
	game->minimap.border_y = top;
	while (game->minimap.border_y <= bottom)
	{
		put_pixel(game, left, game->minimap.border_y, 0x47ECFF);
		put_pixel(game, right, game->minimap.border_y, 0x47ECFF);
		game->minimap.border_y++;
	}
}

void	draw_compass(t_game *game)
{
	int	left;
	int	right;
	int	top;
	int	bottom;

	left = game->minimap.origin_x;
	top = game->minimap.origin_y;
	right = left + game->map.width * game->minimap.tile_size;
	bottom = top + game->map.height * game->minimap.tile_size;
	game->minimap.center_x = (left + right) / 2;
	game->minimap.center_y = (top + bottom) / 2;
	mlx_string_put(game->window.mlx, game->window.window,
		game->minimap.center_x - 4, top - 4, 0x33F5FF, "N");
	mlx_string_put(game->window.mlx, game->window.window,
		game->minimap.center_x - 4, bottom + 14, 0x33F5FF, "S");
	mlx_string_put(game->window.mlx, game->window.window,
		left - 10, game->minimap.center_y + 4, 0x33F5FF, "W");
	mlx_string_put(game->window.mlx, game->window.window,
		right + 5, game->minimap.center_y + 4, 0x33F5FF, "E");
}

int	ft_get_door_color(t_game *game, int x, int y)
{
	t_door	*door;

	door = ft_get_door_at(game, x, y);
	if (door && door->is_open)
		return (0x8FBF8F);
	return (0x25B4D9);
}
