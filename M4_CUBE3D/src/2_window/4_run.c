/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   4_run.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 10:00:40 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/08 11:22:12 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

int	ft_run_window(t_game *game)
{
	ft_init_player_dir(game);
	ft_load_all_tex(game);
	ft_cast_rays(game);
	ft_init_draw(game);
	if (!game || !game->window.mlx || !game->window.window)
	{
		if (game->window.mlx)
			free(game->window.mlx);
		if (game->window.window)
			mlx_destroy_window(game->window.mlx, game->window.window);
		return (printf("Error : Invalid game or window pointer\n"), ERROR);
	}
	mlx_put_image_to_window(game->window.mlx, game->window.window,
		game->img.img_ptr, 0, 0);
	mlx_hook(game->window.window, 2, 1L << 0, (void *)ft_key_hook, game);
	mlx_hook(game->window.window, 3, 1L << 1, (void *)ft_key_release, game);
	game->mouse_enabled = 0;
	game->mouse_ignore = 1;
	mlx_mouse_move(game->window.mlx, game->window.window, WINDOW_WIDTH / 2,
		WINDOW_HEIGHT / 2);
	mlx_hook(game->window.window, 6, 1L << 6, (void *)ft_mouse_rotate, game);
	mlx_hook(game->window.window, 17, 0, (void *)ft_close_and_exit, game);
	mlx_loop_hook(game->window.mlx, (void *)ft_game_loop, game);
	mlx_loop(game->window.mlx);
	return (SUCCESS);
}
