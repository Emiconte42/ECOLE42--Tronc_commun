/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   5_close.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 11:41:51 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/07 14:43:37 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static int	destroy_img(t_game *game)
{
	if (game->window.mlx && game->img.img_ptr)
		mlx_destroy_image(game->window.mlx, game->img.img_ptr);
	if (game->tex.so.img_ptr)
		mlx_destroy_image(game->window.mlx, game->tex.so.img_ptr);
	if (game->tex.no.img_ptr)
		mlx_destroy_image(game->window.mlx, game->tex.no.img_ptr);
	if (game->tex.we.img_ptr)
		mlx_destroy_image(game->window.mlx, game->tex.we.img_ptr);
	if (game->tex.ea.img_ptr)
		mlx_destroy_image(game->window.mlx, game->tex.ea.img_ptr);
	if (game->tex.waepon.img_ptr)
		mlx_destroy_image(game->window.mlx, game->tex.waepon.img_ptr);
	if (game->tex.waepon_shoot.img_ptr)
		mlx_destroy_image(game->window.mlx, game->tex.waepon_shoot.img_ptr);
	if (game->window.mlx && game->window.window)
		mlx_destroy_window(game->window.mlx, game->window.window);
	return (SUCCESS);
}

void	ft_close_window(t_game *game)
{
	int	i;

	i = 0;
	if (!game)
	{
		printf("Error : close_window: game is NULL\n");
		exit(ERROR);
	}
	while (i < DOOR_ANIM_FRAMES)
	{
		if (game->tex.door[i].img_ptr)
			mlx_destroy_image(game->window.mlx, game->tex.door[i].img_ptr);
		i++;
	}
	if (destroy_img(game) != SUCCESS)
		return ;
	if (game->window.mlx)
	{
		mlx_destroy_display(game->window.mlx);
		free(game->window.mlx);
	}
	ft_gc_free(&game->gc);
}

int	ft_close_and_exit(t_game *game)
{
	ft_close_window(game);
	exit(0);
}
