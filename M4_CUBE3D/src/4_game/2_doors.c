/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_doors.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 10:25:31 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/06 10:51:14 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

t_door	*ft_get_door_at(t_game *game, int x, int y)
{
	int	i;

	i = 0;
	while (i < game->doors_count)
	{
		if (game->doors[i].x == x && game->doors[i].y == y)
			return (&game->doors[i]);
		i++;
	}
	return (NULL);
}

void	ft_toggle_door(t_game *game)
{
	int		door_x;
	int		door_y;
	t_door	*door;

	door_x = (int)(game->player.pos_x + game->player.dir_x);
	door_y = (int)(game->player.pos_y + game->player.dir_y);
	door = ft_get_door_at(game, door_x, door_y);
	if (door)
		door->is_open = !door->is_open;
}

void	ft_update_doors(t_game *game)
{
	int		i;
	t_door	*door;

	i = 0;
	while (i < game->doors_count)
	{
		door = &game->doors[i];
		if (door->is_open && door->frame < DOOR_ANIM_FRAMES - 1)
			door->frame++;
		else if (!door->is_open && door->frame > 0)
			door->frame--;
		i++;
	}
}
