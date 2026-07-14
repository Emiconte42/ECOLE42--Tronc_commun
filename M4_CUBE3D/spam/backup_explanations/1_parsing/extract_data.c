/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extract_data.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 11:22:41 by hbelleuv          #+#    #+#             */
/*   Updated: 2026/07/06 16:22:57 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"
#include <stdio.h>
#include <unistd.h>

int	ft_is_begin_map(char *line)
{
	int	i;

	i = 0;
	// si il y des tab ou espace on avance
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	// si la ligne est vide, ce n'est pas le debut de la map
	if (line[i] == '\0' || line[i] == '\n')
		return (0);
	// si on trouve un 1 ou 0 alors c'est que c'est le debut de la map
	if (line[i] == '0' || line[i] == '1')
		return (1);
	return (0);
}

int	ft_append_line_to_map(t_game *game, char *line)
{
	char	**new_grid;
	int		i;
	int		len;

	// verif de la map qu'il n'y ait rien apres
	i = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	if (ft_strncmp(&line[i], "NO ", 3) == 0 || ft_strncmp(&line[i], "SO ", 3) == 0
		|| ft_strncmp(&line[i], "WE ", 3) == 0
		|| ft_strncmp(&line[i], "EA ", 3) == 0
		|| ft_strncmp(&line[i], "F ", 2) == 0
		|| ft_strncmp(&line[i], "C ", 2) == 0)
	{
		printf("Error : map not last\n");
		return (0);
	}
	// creation d'un tableau dynamique qui grandi a chaque nouvelle ligne
	// + 2 : pour la nouvelle ligne + le NULL a la fin
	new_grid = ft_gc_calloc(game->map.height + 2, sizeof(char *), &game->gc);
	if (!new_grid)
	{
		printf("Error : incorrect map allocation\n");
		// ft_gc_free(&game->gc);
		ft_close_window(game);
		exit(1);
	}
	// copie de l'ancien tableau vers le nouveau
	i = 0;
	while (i < game->map.height)
	{
		new_grid[i] = game->map.grid[i];
		i++;
	}
	// calcule de la largeur maximale + enleve le \n a la fin avant de dup
	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
	// copie de la nouvelle ligne lue
	new_grid[i] = ft_gc_strdup(line, &game->gc);
	new_grid[i + 1] = NULL;
	// MIJ de la grid avec la nouvelle et incrementation pour passser a la prochaine ligne
	game->map.grid = new_grid;
	game->map.height++;
	// recalcule la longueur sans \n
	len = ft_strlen(line);
	// ajoute la donnee a la struct
	if (len > game->map.width)
		game->map.width = len;
	return (1);
}

int	parse_param(t_game *game, char *line)
{
	int	i;

	i = 0;
	// On ignore les espaces
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	// si la ligne est vide on l'ignore
	if (line[i] == '\0')
		return (0);
	// detecte les identifiants exactes
	if (ft_strncmp(&line[i], "NO ", 3) == 0)
	{
		// la texture de NO a deja ete definie
		if (game->tex.no_path != NULL)
		{
			printf("Error : texture NO defined twice\n");
			return (0);
		}
		else if (ft_parse_texture(game, &game->tex.no_path, &line[i + 3]) == 0)
			return (0);
	}
	else if (ft_strncmp(&line[i], "SO ", 3) == 0)
	{
		if (game->tex.so_path != NULL)
		{
			printf("Error : texture SO defined twice\n");
			return (0);
		}
		else if (ft_parse_texture(game, &game->tex.so_path, &line[i + 3]) == 0)
			return (0);
	}
	else if (ft_strncmp(&line[i], "WE ", 3) == 0)
	{
		if (game->tex.we_path != NULL)
		{
			printf("Error : texture WE defined twice\n");
			return (0);
		}
		else if (ft_parse_texture(game, &game->tex.we_path, &line[i + 3]) == 0)
			return (0);
	}
	else if (ft_strncmp(&line[i], "EA ", 3) == 0)
	{
		if (game->tex.ea_path != NULL)
		{
			printf("Error : texture EA defined twice\n");
			return (0);
		}
		else if (ft_parse_texture(game, &game->tex.ea_path, &line[i + 3]) == 0)
			return (0);
	}
	else if (!game->tex.no_path || !game->tex.so_path
		|| !game->tex.we_path || !game->tex.ea_path)
	{
		printf("Error : missing texture path\n");
		return (0);
	}
	else if (ft_strncmp(&line[i], "F ", 2) == 0)
	{
		if (game->defined_floor)
		{
			printf("Error : floor color defined twice\n");
			return (0);
		}
		if (ft_parse_color(game, &game->floor, &line[i + 2]) == 0)
			return (0);
		game->defined_floor = 1;
	}
	else if (ft_strncmp(&line[i], "C ", 2) == 0)
	{
		if (game->defined_ceiling)
		{
			printf("Error : ceiling color defined twice\n");
			return (0);
		}
		if (ft_parse_color(game, &game->ceiling, &line[i + 2]) == 0)
			return (0);
		game->defined_ceiling = 1;
	}
	else
	{
		if (line[i] == '\n')
			i++;
		else
		{
			printf("Error : unknown identifier\n");
			return (0);
		}
	}
	return (1);
}

int	ft_parse_cub_file(char *filename, t_game *game)
{
	int		fd;
	char	*line;
	int		is_map;

	is_map = 0; // flag qui passe a 1 des qu'on detecte la map
	printf("Erreur : map ouverte 0\n");
	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		printf("Erreur : map ouverte 1\n");
		return (0);
	}
	line = ft_gc_get_next_line(&game->gc, fd);
	while (line != NULL)
	{
		if (is_map == 0 && ft_is_begin_map(line))
			is_map = 1;
		if (is_map == 1)
		{
			if (ft_append_line_to_map(game, line) == 0) // ajouter la ligne a grid
			{
				close(fd);
				printf("Erreur : map ouverte 2\n");
				return (0);
			}
		}
		else
		{
			if (parse_param(game, line) == 0) // Analyser NO, SO, F, C, etc
			{
				close(fd);
				printf("Erreur : map ouverte 3\n");
				return (0);
			}
		}
		line = ft_gc_get_next_line(&game->gc, fd);
	}
	close(fd);
	if (ft_validate_map(game) == 0)
	{
		printf("Erreur : map ouverte 4\n");
		return (0);
	}
	else if (ft_check_walls(game) == 0)
	{
		printf("Erreur : map ouverte 5\n");
		return (0);
	}
	else if ((!game->floor.b && !game->floor.r && !game->floor.g)
	|| (!game->ceiling.r && !game->ceiling.g && !game->ceiling.b))
	{
		printf("Error : missing color\n");
		printf("Erreur : map ouverte 6\n");
		return (0);
	}
	return (1);
}
