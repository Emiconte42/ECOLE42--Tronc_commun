/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 14:02:41 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/06 16:24:02 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"
#include <unistd.h>

void	ft_print_game(t_game *game)
{
	int	i;

	i = 0;

	printf("=== DEBUG PARSING ETAPE 1 ===\n");

	// Affichage des textures
	printf("NO: %s\n", game->tex.no_path);
	printf("SO: %s\n", game->tex.so_path);
	printf("WE: %s\n", game->tex.we_path);
	printf("EA: %s\n", game->tex.ea_path);

	// Affichage des couleurs
	printf("Floor (F): R:%d, G:%d, B:%d\n", game->floor.r, game->floor.g, game->floor.b);
	printf("Ceiling (C): R:%d, G:%d, B:%d\n", game->ceiling.r, game->ceiling.g, game->ceiling.b);

	// Affichage de la carte
	printf("\n=== MAP (Hauteur: %d, Largeur max: %d) ===\n", game->map.height, game->map.width);
	if (game->map.grid)
	{
		while (i < game->map.height)
		{
			printf("%s", game->map.grid[i]);
			i++;
		}
	}
	else
	{
		printf("(Aucune carte chargée)\n");
	}
	printf("=============================\n");
}

int main(int argc, char **argv)
{
	t_game game;

	ft_memset(&game, 0, sizeof(t_game));
	if (argc != 2)
	{
		printf("Error : Usage : './cub3d <carte.cub>'\n");
		return (1);
	}
	else if (ft_check_extension(argv[1]) == 0)
	{
		printf("Error : you must submit a .cub file\n");
		ft_close_window(&game);
		exit(1);
	}
	// 1. Initialisation de la structure et du GC
	// game.gc = ft_gc_init();
	// 2. Lancement du parsing
	if (ft_parse_cub_file(argv[1], &game) == 0)
	{
		// ft_gc_free(&game.gc);
		ft_close_window(&game);
		exit(1);
	}

	// 3. Test
	//	ft_print_game(&game);
	
	// 4. Lancement de la fenetre
	if (ft_init_window(&game) != SUCCESS)
		return (printf("Error : init window\n"), ERROR);
	if (ft_init_img(&game) != SUCCESS) {
		ft_close_window(&game);
		return (printf("Error : init img\n"), ERROR);
	}
	if (ft_run_window(&game) != SUCCESS) {
		ft_close_window(&game);
		return (printf("Error : run window\n"), ERROR);
	}
	// 5. Libération totale de la mémoire (Garbage Collector de l'Etape 0)
	ft_close_window(&game);
	// ft_gc_free(&game.gc); // pas utile car deja free dans close_window
	return (0);
}
