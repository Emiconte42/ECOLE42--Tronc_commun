/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   1_init_window.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 11:53:33 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/07 12:15:13 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

int	ft_init_window(t_game *game)
{
	game->window.mlx = mlx_init();
	if (!game->window.mlx)
		return (printf("Error : connection mlx\n"), ERROR);
	game->window.window = mlx_new_window(game->window.mlx, WINDOW_WIDTH,
			WINDOW_HEIGHT, "cub3d");
	if (!game->window.window)
	{
		free(game->window.mlx);
		return (printf("Error : init window\n"), ERROR);
	}
	game->minimap.origin_x = 15;
	game->minimap.origin_y = 15;
	return (SUCCESS);
}

int	ft_init_img(t_game *game)
{
	game->img.img_ptr = mlx_new_image(game->window.mlx, WINDOW_WIDTH,
			WINDOW_HEIGHT);
	if (!game->img.img_ptr)
		return (printf("Error : create image\n"), ERROR);
	game->img.img_addr = mlx_get_data_addr(game->img.img_ptr,
			&game->img.bits_per_pixels, &game->img.size_line,
			&game->img.endian);
	return (SUCCESS);
}
