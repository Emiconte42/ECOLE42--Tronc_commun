/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   3_render.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 10:00:38 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/30 14:15:43 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

// Dessine un pixel de couleur a la position x et y dans le buffer image
static void	put_pixel(t_game *game, int x, int y, int color)
{
	char	*pixel;
	
	// Si les coordonnees sont en dehors de la fenetre, ne rien dessiner
	if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
		return ;
	
	// Calcule l'adresse memoire exacte du pixel
	// (ligne y * taille d'une ligne) + (colonne x * nombre d'octets par pixel)
	pixel = game->img.img_addr + (y * game->img.size_line + x *
			(game->img.bits_per_pixels / 8));

	// Ecrit la couleur a cette adresse memoire
	*(unsigned int *)pixel = color;
}

// Redessine entierement l'image et l'affiche dans la fenetre
void	redraw(t_game *game)
{
	// Remet tout le buffer image a 0 (efface l'image precedente)
	ft_memset(game->img.img_addr, 0, game->img.size_line * WINDOW_HEIGHT);

	// Redessine la map dans le buffer
	draw_map(game);

	// Affiche le buffer image dans la fenetre
	mlx_put_image_to_window(game->window.mlx, game->window.window,
		game->img.img_ptr, 0, 0);
}

// Dessine un carre de MINIMAP_TILE x MINIMAP_TILE pixels a la position x, y
static int	draw_square(t_game *game, int x, int y, int color)
{
	int	i;
	int	j;

	i = 0;
	while (i < MINIMAP_TILE)
	{
		j = 0;
		while (j < MINIMAP_TILE)
		{
			// Dessine chaque pixel du carre un par un
			put_pixel(game, x + j, y + i, color);
			j++;
		}
		i++;
	}
	return (SUCCESS);
}

// Dessine la map (murs et sol) ainsi que la position du joueur
int	draw_map(t_game *game)
{
	int	x;
	int	y;
	int	px; // position x du joueur a l'ecran
	int	py; // position y du joueur a l'ecran

	y = 0;
	// Verifie que la grille de la map existe avant de la parcourir
	if (!game->map.grid)
	{
		printf("Error : map grid is NULL\n");
		return (ERROR);
	}
	while (y < game->map.height)
	{
		x = 0;
		while (x < game->map.width)
		{
			if (game->map.grid[y][x] == '1') // Case mur : dessine un carre blanc
				draw_square(game, game->minimap.origin_x + x * MINIMAP_TILE, game->minimap.origin_y + y * MINIMAP_TILE, 0xFFFFFF);
			else // Sinon Case sol : dessine un carre noir
				draw_square(game, game->minimap.origin_x + x * MINIMAP_TILE, game->minimap.origin_y + y * MINIMAP_TILE, 0x000000);
			x++;
		}
		y++;
	}

	// Calcule la position du joueur a l'ecran a partir de sa position dans la map
	px = game->minimap.origin_x + (int)game->player.pos_x * MINIMAP_TILE;
	py = game->minimap.origin_y + (int)game->player.pos_y * MINIMAP_TILE;
	draw_square(game, px, py, 0xFF0000); // Dessine le joueur sous forme de carre rouge

	return (SUCCESS);
}
