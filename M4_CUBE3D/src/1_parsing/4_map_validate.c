/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   4_map_validate.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 18:10:52 by hbelleuv          #+#    #+#             */
/*   Updated: 2026/07/14 12:12:43 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static int	ft_check_map_char(t_game *game, int i, int j, int *player_count)
{
	char	c;

	c = game->map.grid[i][j];
	if (c != '0' && c != '1' && c != 'N' && c != 'S'
		&& c != 'W' && c != 'E' && c != 'D' && c != ' ' && c != '\t')
	{
		printf("Error : invalid character in the map\n");
		return (0);
	}
	else if (c == 'N' || c == 'S' || c == 'W' || c == 'E')
	{
		(*player_count)++;
		game->player.spawn_dir = c;
		game->player.pos_x = j + 0.5;
		game->player.pos_y = i + 0.5;
	}
	return (1);
}

static int	ft_check_all_lines(t_game *game, int *player_count)
{
	int	i;
	int	j;

	i = 0;
	while (i < game->map.height)
	{
		j = 0;
		if (game->map.grid[i][j] == '\n' || game->map.grid[i][j] == '\0')
		{
			printf("Error : empty line detected in the map\n");
			return (0);
		}
		while (game->map.grid[i][j])
		{
			if (ft_check_map_char(game, i, j, player_count) == 0)
				return (0);
			j++;
		}
		i++;
	}
	return (1);
}

int	ft_validate_map(t_game *game)
{
	int		player_count;

	player_count = 0;
	if (game->map.grid == NULL || game->map.height == 0)
	{
		printf("Error : no map found in the file\n");
		return (0);
	}
	ft_fix_map_spaces(game);
	if (ft_check_all_lines(game, &player_count) == 0)
		return (0);
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

int	ft_check_surrounding(t_game *game, int i, int j)
{
	int	len_prev;
	int	len_next;

	if (i == 0 || j == 0 || i == game->map.height - 1
		|| game->map.grid[i][j + 1] == '\0')
	{
		printf("Error : map is not closed\n");
		return (0);
	}
	len_prev = ft_strlen(game->map.grid[i - 1]);
	len_next = ft_strlen(game->map.grid[i + 1]);
	if (j >= len_prev || j >= len_next)
	{
		printf("Error : map is not closed\n");
		return (0);
	}
	if (game->map.grid[i][j - 1] == ' '
		|| game->map.grid[i][j + 1] == ' '
		|| game->map.grid[i - 1][j] == ' '
		|| game->map.grid[i + 1][j] == ' ')
	{
		printf("Error : map is not closed\n");
		return (0);
	}
	return (1);
}

int	ft_check_walls(t_game *game)
{
	int	i;
	int	j;

	i = 0;
	while (i < game->map.height)
	{
		j = 0;
		while (game->map.grid[i][j])
		{
			if (!ft_check_cell(game, i, j))
				return (0);
			j++;
		}
		i++;
	}
	return (1);
}
