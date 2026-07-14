/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   1_init_window.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 11:53:33 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/30 11:46:00 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

// Initialise la connection avec la mlx et creer la fenetre
int	init_window(t_game *game)
{
	// initialiser la connection avec la MLX graphique
	game->window.mlx = mlx_init();

	// Si echec, afficher un message d'erreur et retourner ERROR
	if (!game->window.mlx)
		return (printf("Error : connection mlx\n"), ERROR);

	// Creer une fenetre aux dimensions WINDOW_WIDTH x WINDOW_HEIGHT avec pour titre "cub3d"
	game->window.window = mlx_new_window(game->window.mlx, WINDOW_WIDTH,
			WINDOW_HEIGHT, "cub3d"); // creer une fenetre de 1920 x 1080

	// Si echec, afficher un message d'erreur et retourner ERROR
	if (!game->window.window)
		return (printf("Error : init window\n"), ERROR);
	return (SUCCESS);
}

// Creer l'image buffer et recupere son adresse memoire
int	init_img(t_game *game)
{
	// Creer une nouvelle image aux dimensions de la fenetre
	game->img.img_ptr = mlx_new_image(game->window.mlx, WINDOW_WIDTH,
			WINDOW_HEIGHT);

	// Si echec, afficher un message d'erreur et retourner ERROR
	if (!game->img.img_ptr)
		return (printf("Error : create image\n"), ERROR);

	// Recuperer l'adresse memoire de l'image ainsi que ses infos
	// (nombre de bits par pixel, taille d'une ligne, endianness)
	// Endianness : Recuperer l'ordre des octets du systeme pour ecrire correctement les pixels dans le buffer
	game->img.img_addr = mlx_get_data_addr(game->img.img_ptr,
			&game->img.bits_per_pixels, &game->img.size_line,
			&game->img.endian);
	return (SUCCESS);
}
