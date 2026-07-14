/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   1_render.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 10:00:38 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/07 11:06:19 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

// Dessine une pixel de couleur a la position x et y
void	put_pixel(t_game *game, int x, int y, int color)
{
	char	*pixel;

	if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
		return ;
	pixel = game->img.img_addr + (y * game->img.size_line + x *
			(game->img.bits_per_pixels / 8));
	*(unsigned int *)pixel = color;
}

// Redessine entierement l'image et l'affiche dans la fenetre
void	ft_redraw(t_game *game)
{
	ft_memset(game->img.img_addr, 0, game->img.size_line * WINDOW_HEIGHT);
	ft_cast_rays(game);
	ft_init_draw(game);
	ft_draw_weapon(game);
	mlx_put_image_to_window(game->window.mlx, game->window.window,
		game->img.img_ptr, 0, 0);
	draw_compass(game);
}

int	ft_init_draw(t_game *game)
{
	if (!game->map.grid)
	{
		printf("Error : map grid is NULL\n");
		return (ERROR);
	}
	if (game->map.width > game->map.height || game->map.width == game->map.height)
		game->minimap.size_map = game->map.width;
	else
		game->minimap.size_map = game->map.height;
	game->minimap.tile_size = (int)((double)MINIMAP_MAX_SIZE / (double)game->minimap.size_map);
	if (draw_map(game) != SUCCESS)
		return (printf("Error : draw map problem\n"), ERROR);
	if (draw_player(game) != SUCCESS)
		return (printf("Error : draw map problem\n"), ERROR);
	ft_draw_weapon(game);
	return (SUCCESS);
}
