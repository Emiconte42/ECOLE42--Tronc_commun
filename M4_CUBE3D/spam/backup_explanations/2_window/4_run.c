/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   4_run.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 10:00:40 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/30 12:20:39 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

// enregistre les hooks et lance la boucle des evenements
int	run_window(t_game *game)
{
	// Verifie que les pointeurs essentiels existent avant de continuer
	if (!game || !game->window.mlx || !game->window.window)
	{
		printf("Error : Invalid game or window pointer\n");
		return (ERROR);
	}

	// Dessine la map une premiere fois dans le buffer image
	draw_map(game);

	// Affiche le buffer image dans la fenetre
	mlx_put_image_to_window(game->window.mlx, game->window.window, 
		game->img.img_ptr, 0, 0);

	// Enregistre le hook sur les touches du clavier
	// (appelle key_hook a chaque pression)
	mlx_key_hook(game->window.window, key_hook, game);

	// fermer avec la croix
	mlx_hook(game->window.window, 17, 0, (void *)close_window, game);

	// Lancer la boucle des evenements MLX
	mlx_loop(game->window.mlx);
	return (SUCCESS);
}
