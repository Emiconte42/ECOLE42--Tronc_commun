/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_hooks.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:59:41 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/07 14:40:41 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static int	handle_key(int keycode, t_game *game)
{
	if (keycode == KEY_D)
		game->keys.strafe_left = 1;
	if (keycode == KEY_A)
		game->keys.strafe_right = 1;
	if (keycode == KEY_W || keycode == KEY_UP)
		game->keys.move_forward = 1;
	if (keycode == KEY_S || keycode == KEY_DOWN)
		game->keys.move_backward = 1;
	if (keycode == KEY_LEFT)
		game->keys.rotate_left = 1;
	if (keycode == KEY_RIGHT)
		game->keys.rotate_right = 1;
	if (keycode == KEY_F)
		game->keys.is_shooting = 1;
	return (SUCCESS);
}

static int	ft_key_m(int keycode, t_game *game)
{
	if (keycode == KEY_M)
	{
		game->mouse_enabled = !game->mouse_enabled;
		if (game->mouse_enabled)
		{
			mlx_mouse_hide(game->window.mlx, game->window.window);
			game->mouse_ignore = 1;
			mlx_mouse_move(game->window.mlx, game->window.window,
				WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
		}
		else
			mlx_mouse_show(game->window.mlx, game->window.window);
	}
	return (SUCCESS);
}

int	ft_key_hook(int keycode, void *param)
{
	t_game	*game;

	game = (t_game *)param;
	if (keycode == KEY_ESC)
	{
		ft_close_window(game);
		exit(0);
	}
	if (keycode == KEY_SPACE)
		ft_toggle_door(game);
	if (handle_key(keycode, game) != SUCCESS)
		return (ERROR);
	if (ft_key_m(keycode, game) != SUCCESS)
		return (ERROR);
	return (SUCCESS);
}

int	ft_key_release(int keycode, void *param)
{
	t_game	*game;

	game = (t_game *)param;
	if (keycode == KEY_D)
		game->keys.strafe_left = 0;
	else if (keycode == KEY_A)
		game->keys.strafe_right = 0;
	else if (keycode == KEY_W || keycode == KEY_UP)
		game->keys.move_forward = 0;
	else if (keycode == KEY_S || keycode == KEY_DOWN)
		game->keys.move_backward = 0;
	else if (keycode == KEY_LEFT)
		game->keys.rotate_left = 0;
	else if (keycode == KEY_RIGHT)
		game->keys.rotate_right = 0;
	if (keycode == KEY_F)
		game->keys.is_shooting = 0;
	return (SUCCESS);
}

void	ft_game_loop(void *param)
{
	t_game	*game;

	game = (t_game *)param;
	if (game->keys.strafe_right == 1)
		ft_move_axis(game, game->player.plane_x, game->player.plane_y, -1);
	if (game->keys.strafe_left == 1)
		ft_move_axis(game, game->player.plane_x, game->player.plane_y, 1);
	if (game->keys.move_forward == 1)
		ft_move_axis(game, game->player.dir_x, game->player.dir_y, 1);
	if (game->keys.move_backward == 1)
		ft_move_axis(game, game->player.dir_x, game->player.dir_y, -1);
	else if (game->keys.rotate_right == 1)
		ft_rotate(game, ANGLE_SPEED);
	else if (game->keys.rotate_left == 1)
		ft_rotate(game, -ANGLE_SPEED);
	ft_update_doors(game);
	ft_redraw(game);
}
