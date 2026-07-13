/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_parse_params.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 12:03:09 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/07 12:03:14 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static int	ft_check_texture(t_game *game, char *str, char **path)
{
	if (*path != NULL)
	{
		printf("Error : texture defined twice\n");
		return (0);
	}
	return (ft_parse_texture(game, path, str + 3));
}

static int	ft_parse_texture_line(t_game *game, char *str)
{
	if (ft_strncmp(str, "NO ", 3) == 0)
		return (ft_check_texture(game, str, &game->tex.no_path));
	else if (ft_strncmp(str, "SO ", 3) == 0)
		return (ft_check_texture(game, str, &game->tex.so_path));
	else if (ft_strncmp(str, "WE ", 3) == 0)
		return (ft_check_texture(game, str, &game->tex.we_path));
	else if (ft_strncmp(str, "EA ", 3) == 0)
		return (ft_check_texture(game, str, &game->tex.ea_path));
	return (-1);
}

static int	ft_parse_color_line(t_game *game, char *str)
{
	if (ft_strncmp(str, "F ", 2) == 0)
	{
		if (game->defined_floor)
		{
			printf("Error : floor color defined twice\n");
			return (0);
		}
		game->defined_floor = 1;
		return (ft_parse_color(game, &game->floor, str + 2));
	}
	else if (ft_strncmp(str, "C ", 2) == 0)
	{
		if (game->defined_ceiling)
		{
			printf("Error : ceiling color defined twice\n");
			return (0);
		}
		game->defined_ceiling = 1;
		return (ft_parse_color(game, &game->ceiling, str + 2));
	}
	return (-1);
}

int	ft_parse_param(t_game *game, char *line)
{
	int	i;
	int	ret;

	i = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	if (line[i] == '\0' || line[i] == '\n')
		return (1);
	ret = ft_parse_texture_line(game, line);
	if (ret != -1)
		return (ret);
	ret = ft_parse_color_line(game, line);
	if (ret != -1)
		return (ret);
	printf("Error : unknown identifier\n");
	return (0);
}

int	ft_check_missing_elements(t_game *game)
{
	if (!game->tex.no_path || !game->tex.so_path
		|| !game->tex.we_path || !game->tex.ea_path)
	{
		printf("Error : missing texture path\n");
		return (0);
	}
	else if (!game->defined_floor || !game->defined_ceiling)
	{
		printf("Error : missing color\n");
		return (0);
	}
	if (ft_validate_map(game) == 0)
		return (0);
	else if (ft_check_walls(game) == 0)
		return (0);
	return (1);
}
