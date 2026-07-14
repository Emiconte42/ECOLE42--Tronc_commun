/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   1_movements.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 16:46:31 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/06 10:52:06 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

void	ft_move_axis(t_game *game, double dx, double dy, double sign)
{
	double	new_x;
	double	new_y;

	new_x = game->player.pos_x + dx * MOVE_SPEED * sign;
	new_y = game->player.pos_y + dy * MOVE_SPEED * sign;
	if (ft_can_move(game, new_x, game->player.pos_y) == SUCCESS)
		game->player.pos_x = new_x;
	if (ft_can_move(game, game->player.pos_x, new_y) == SUCCESS)
		game->player.pos_y = new_y;
}

int	ft_is_wall(t_game *game, double x, double y)
{
	int		convert_x;
	int		convert_y;
	t_door	*door;

	convert_x = (int)x;
	convert_y = (int)y;
	if (x < 0 || y < 0)
		return (ERROR);
	if (convert_x >= game->map.width || convert_y >= game->map.height)
		return (ERROR);
	if (game->map.grid[convert_y][convert_x] == '1')
		return (ERROR);
	if (game->map.grid[convert_y][convert_x] == 'D')
	{
		door = ft_get_door_at(game, convert_x, convert_y);
		if (door && door->frame < DOOR_ANIM_FRAMES - 1)
			return (ERROR);
	}
	return (SUCCESS);
}

int	ft_can_move(t_game *game, double new_x, double new_y)
{
	if (ft_is_wall(game, new_x - PLAYER_RADIUS, new_y) == ERROR)
		return (ERROR);
	if (ft_is_wall(game, new_x + PLAYER_RADIUS, new_y) == ERROR)
		return (ERROR);
	if (ft_is_wall(game, new_x, new_y - PLAYER_RADIUS) == ERROR)
		return (ERROR);
	if (ft_is_wall(game, new_x, new_y + PLAYER_RADIUS) == ERROR)
		return (ERROR);
	return (SUCCESS);
}

void	ft_rotate(t_game *game, double angle)
{
	double	old_dir_x;
	double	old_plane_x;

	old_dir_x = game->player.dir_x;
	game->player.dir_x = game->player.dir_x * cos(angle) - game->player.dir_y * sin(angle);
	game->player.dir_y = old_dir_x * sin(angle) + game->player.dir_y * cos(angle);
	old_plane_x = game->player.plane_x;
	game->player.plane_x = game->player.plane_x * cos(angle) - game->player.plane_y * sin(angle);
	game->player.plane_y = old_plane_x * sin(angle) + game->player.plane_y * cos(angle);
}