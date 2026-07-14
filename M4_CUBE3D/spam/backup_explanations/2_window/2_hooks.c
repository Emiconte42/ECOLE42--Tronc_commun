/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_hooks.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 09:59:41 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/01 16:17:52 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

// Ferme proprement la fenetre, libere les ressources mlx et quitte le programme
int	close_window(t_game *game)
{
	// Pointeur game invalide, afficher une erreur et quitter
	if (!game)
	{
		printf("Error : close_window: game is NULL\n");
		exit(ERROR);
	}
	if (game->window.mlx && game->img.img_ptr)
		// Detruit l'image creee dans init_img
		mlx_destroy_image(game->window.mlx, game->img.img_ptr);
	if (game->window.mlx && game->window.window)
		// Detruit la fenetre graphique
		mlx_destroy_window(game->window.mlx, game->window.window);
	if (game->window.mlx)
		// Detruit la connection avec le systeme graphique (mlx_init())
		mlx_destroy_display(game->window.mlx);
	ft_gc_free(&game->gc); // Libere toutes les allocations du GC
	exit(SUCCESS);
}

// Gere le deplacement du joueur selon la touche appuyee
static int	handle_key(int keycode, t_game *game)
{
	if (keycode == KEY_A || keycode == KEY_LEFT)
		game->player.pos_x -= 0.1; // Deplace le joueur vers la gauche
	else if (keycode == KEY_D || keycode == KEY_RIGHT)
		game->player.pos_x += 0.1; // Deplace le joueur vers la droite
	else if (keycode == KEY_W || keycode == KEY_UP)
		game->player.pos_y -= 0.1; // Deplace le joueur vers le haut ??
	else if (keycode == KEY_S || keycode == KEY_DOWN)
		game->player.pos_y += 0.1; // Deplace le joueur vers le bas ??
	else
		// Touche non geree, ne rien faire et retourner SUCCESS
		return (SUCCESS);
	// Une touche de deplacement a ete geree, retourner ERROR pour signaler qu'il faut redessiner
	return (ERROR);
}

// Hook appele par la MLX a chaque pression de touche
int	key_hook(int keycode, void *param)
{
	t_game	*game;

	// param est un void*, on le recast en t_game* pour acceder aux donnees du jeu
	game = (t_game *)param;

	// Si la touche appuyee est ECHAP, on ferme proprement la fenetre
	if (keycode == KEY_ESC)
	{
		close_window(game);
		return (ERROR);
	}

	// Si le joueur a bouge, redessiner la scene
	if (handle_key(keycode, game) != SUCCESS)
		redraw(game);
	return (SUCCESS);
}
