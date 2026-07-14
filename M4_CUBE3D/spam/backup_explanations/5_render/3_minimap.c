/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   3_minimap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 11:41:03 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/07 11:15:45 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static void	draw_map_border(t_game *game)
{
	int	x;
	int	y;
	int	left;
	int	right;
	int	top;
	int	bottom;

	left = game->minimap.origin_x;
	top = game->minimap.origin_y;
	right = left + game->map.width * game->minimap.tile_size;
	bottom = top + game->map.height * game->minimap.tile_size;

	x = left;
	while (x <= right)
	{
		put_pixel(game, x, top, 0x47ECFF);
		put_pixel(game, x, bottom, 0x47ECFF);
		x++;
	}
	y = top;
	while (y <= bottom)
	{
		put_pixel(game, left, y, 0x47ECFF);
		put_pixel(game, right, y, 0x47ECFF);
		y++;
	}
}

static int	draw_beam(t_game *game)
{
	int		i;
	double	beam_x;
	double	beam_y;

	i = 0;
	while (i < PLAYER_DIR_LEN)
	{
		beam_x = game->player.pos_x + game->player.dir_x * i * BEAM_STEP;
		beam_y = game->player.pos_y + game->player.dir_y * i * BEAM_STEP;
		if (ft_can_move(game, beam_x, beam_y) == ERROR)
			return (SUCCESS);
		put_pixel(game,
			game->minimap.origin_x + (int)(beam_x * game->minimap.tile_size),
			game->minimap.origin_y + (int)(beam_y * game->minimap.tile_size),
			0xFF7869);
		i++;
	}
	return (SUCCESS);
}

static int	draw_square(t_game *game, t_rectangle rectangle)
{
	int	i;
	int	j;

	i = 0;
	while (i < rectangle.size)
	{
		j = 0;
		while (j < rectangle.size)
		{
			put_pixel(game, rectangle.x + j, rectangle.y + i, rectangle.color);
			j++;
		}
		i++;
	}
	return (SUCCESS);
}

void	draw_compass(t_game *game)
{
	int	left;
	int	right;
	int	top;
	int	bottom;
	int	center_x;
	int	center_y;

	left = game->minimap.origin_x;
	top = game->minimap.origin_y;
	right = left + game->map.width * game->minimap.tile_size;
	bottom = top + game->map.height * game->minimap.tile_size;
	center_x = (left + right) / 2;
	center_y = (top + bottom) / 2;

	mlx_string_put(game->window.mlx, game->window.window,
		center_x - 4, top - 4, 0x33F5FF, "N");
	mlx_string_put(game->window.mlx, game->window.window,
		center_x - 4, bottom + 14, 0x33F5FF, "S");
	mlx_string_put(game->window.mlx, game->window.window,
		left - 10, center_y + 4, 0x33F5FF, "W");
	mlx_string_put(game->window.mlx, game->window.window,
		right + 5, center_y + 4, 0x33F5FF, "E");
}

int	draw_player(t_game *game)
{
	int	px;
	int	py;

	t_rectangle	rectangle;

	px = game->minimap.origin_x
		+ (int)(game->player.pos_x * game->minimap.tile_size);
	py = game->minimap.origin_y
		+ (int)(game->player.pos_y * game->minimap.tile_size);
	rectangle.x = px - PLAYER_POINT_SIZE / 2;
	rectangle.y = py - PLAYER_POINT_SIZE / 2;
	rectangle.color = 0xFF0000;
	rectangle.size = PLAYER_POINT_SIZE;
	draw_beam(game);
	draw_square(game, rectangle);
	return (SUCCESS);
}

static int	ft_get_door_color(t_game *game, int x, int y)
{
	t_door	*door;

	door = ft_get_door_at(game, x, y);
	if (door && door->is_open)
		return (0x8FBF8F);
	return (0x25B4D9);
}

// Dessine la map (murs et sol) ainsi que la position du joueur
int	draw_map(t_game *game)
{
	int		x;
	int		y;
	t_rectangle	rectangle;

	y = 0;
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			rectangle.x = game->minimap.origin_x + x * game->minimap.tile_size;
			rectangle.y = game->minimap.origin_y + y * game->minimap.tile_size;
			rectangle.size = game->minimap.tile_size;
			if (game->map.grid[y][x] == '1')
				rectangle.color = 0x3B3B3B;
			else if (game->map.grid[y][x] == 'D')
				rectangle.color = ft_get_door_color(game, x, y);
			else
				rectangle.color = 0x597794;
			draw_square(game, rectangle);
			draw_map_border(game);
			x++;
		}
		y++;
	}
	return (SUCCESS);
}
