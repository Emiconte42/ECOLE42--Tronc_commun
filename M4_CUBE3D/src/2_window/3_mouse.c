/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   3_mouse.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 13:10:08 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/06 14:02:13 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

int	ft_mouse_rotate(int x, int y, t_game *game)
{
	int	delta;

	(void)y;
	if (!game->mouse_enabled)
		return (0);
	if (game->mouse_ignore)
	{
		game->mouse_ignore = 0;
		return (0);
	}
	delta = x - (WINDOW_WIDTH / 2);
	if (delta != 0)
		ft_rotate(game, delta * MOUSE_SENSITIVITY);
	game->mouse_ignore = 1;
	mlx_mouse_move(game->window.mlx, game->window.window,
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	return (0);
}
