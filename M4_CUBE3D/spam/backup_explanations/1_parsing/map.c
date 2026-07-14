/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbelleuv </var/spool/mail/hbelleuv>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 18:10:52 by hbelleuv          #+#    #+#             */
/*   Updated: 2026/06/28 14:27:50 by hbelleuv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"
#include <stdio.h>

int	ft_validate_map(t_game *game)
{
	int		i;
	int		j;
	int		player_count;
	char	c;

	i = 0;
	player_count = 0;
	// verifie si la map existe bien et a ete trouvee
	if (game->map.grid == NULL || game->map.height == 0)
	{
		printf("Error : no map found in the file\n");
		return (0);
	}
	while (i < game->map.height)
	{
		// verifie les lignes vides au milieu de la map
		j = 0;
		if (game->map.grid[i][j] == '\n' || game->map.grid[i][j] == '\0')
		{
			printf("Error : empty line detected in the map\n");
			return (0);
		}
		while (game->map.grid[i][j] != '\0')
		{
			c = game->map.grid[i][j];
			// verifie les char valide dans la map
			if (c != '0' && c != '1' && c != 'N' && c != 'S'
				&& c != 'W' && c != 'E') //&& c != ' ' && c != '\t')
			{
				printf("Error : invalid character in the map\n");
				return (0);
			}
			// compter le joueur + stock ses infos
			else if (c == 'N' || c == 'S' || c == 'W' || c == 'E')
			{
				player_count++;
				game->player.spawn_dir = c;
				game->player.pos_x = i + 0.5;
				game->player.pos_y = j + 0.5;
			}
			j++;
		}
		i++;
	}
	if (player_count == 0)
	{
		printf("Error : no player starting points found\n");
		return (0);
	}
	if (player_count > 1)
	{
		printf("Error : several player starting points found\n");
		return (0);
	}
	return (1);
}

/*
L'idée est de parcourir toute votre carte (game->map.grid).
Dès que vous trouvez un espace vide (0) ou le joueur (N, S, E, W)
vous regardez ses 4 voisins immédiats (Haut, Bas, Gauche, Droite)
La carte est considérée comme "ouverte" (et donc invalide) si l'un de ces voisins est :
- En dehors des limites du tableau (ex: le 0 est sur la toute première ligne ou la toute dernière).
- Un espace (' ').
- Le caractère nul ('\0') de fin de chaîne, ou si la ligne du dessus/dessous est plus courte que la ligne actuelle.
*/

int	ft_check_walls(t_game *game)
{
	int		i;
	int		j;
	int		len_prev;
	int		len_next;
	char	c;

	i = 0;
	while (i < game->map.height)
	{
		j = 0;
		while (game->map.grid[i][j] != '\0')
		{
			c = game->map.grid[i][j];
			// on verifie l'entoure pour le sol (0) et pour la position du joueur
			if (c == '0' || c == 'N' || c == 'S' || c == 'W' || c == 'E')
			{
				// si on est sur les bord de la map (bord haut, bord gauche, bord droite, bord bas)
				if (i == 0 || j == 0 || game->map.grid[i][j + 1] == '\0' || i == game->map.height - 1)
				{
					printf("Error : the map isn't completely surrounded by walls\n");
					return (0);
				}
				// verif des 4 voisins immédiats
				len_prev = ft_strlen(game->map.grid[i - 1]);
				len_next = ft_strlen(game->map.grid[i + 1]);
				// voison de gauche et droite
				if (game->map.grid[i][j - 1] == ' ' || game->map.grid[i][j + 1] == ' ')
					return (0);
				// voisin du haut
				if (j >= len_prev || game->map.grid[i - 1][j] == ' ')
					return (0);
				// voisin du bas
				if (j >= len_next || game->map.grid[i + 1][j] == ' ')
					return (0);
			}
			j++;
		}
		i++;
	}
	return (1);
}
