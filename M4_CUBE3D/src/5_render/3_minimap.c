/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   3_minimap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 11:41:03 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/08 11:33:31 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

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

int	draw_player(t_game *game)
{
	int			px;
	int			py;
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

static void	draw_map_utils(t_game *game, t_rectangle rectangle)
{
	draw_square(game, rectangle);
	draw_map_border(game);
}

int	draw_map(t_game *game)
{
	int			x;
	int			y;
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
			draw_map_utils(game, rectangle);
			x++;
		}
		y++;
	}
	return (SUCCESS);
}
