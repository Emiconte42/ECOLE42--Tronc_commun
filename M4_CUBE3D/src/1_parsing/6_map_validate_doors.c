/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   5_map_validate_doors.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 15:50:12 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/07 12:05:04 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static int	ft_is_walkable(char c)
{
	return (c == '0' || c == 'N' || c == 'S'
		|| c == 'W' || c == 'E');
}

static int	ft_check_door_border(t_game *game, int i, int j)
{
	int	len_prev;
	int	len_next;

	if (i == 0 || j == 0 || game->map.grid[i][j + 1] == '\0'
		|| i == game->map.height - 1)
	{
		printf("Error : a door is on the map border\n");
		return (0);
	}
	len_prev = ft_strlen(game->map.grid[i - 1]);
	len_next = ft_strlen(game->map.grid[i + 1]);
	if (j >= len_prev || j >= len_next)
	{
		printf("Error : a door is on the map border\n");
		return (0);
	}
	return (1);
}

t_doors	ft_check_door_walls(t_game *game, int i, int j)
{
	char	up;
	char	down;
	char	left;
	char	right;

	if (!ft_check_door_border(game, i, j))
		return (DOOR_INVALID);
	up = game->map.grid[i - 1][j];
	down = game->map.grid[i + 1][j];
	left = game->map.grid[i][j - 1];
	right = game->map.grid[i][j + 1];
	if (up == '1' && down == '1'
		&& ft_is_walkable(left)
		&& ft_is_walkable(right))
		return (DOOR_HORIZONTAL);
	if (left == '1' && right == '1'
		&& ft_is_walkable(up)
		&& ft_is_walkable(down))
		return (DOOR_VERTICAL);
	printf("Error : invalid door placement\n");
	return (DOOR_INVALID);
}

int	ft_count_doors(t_game *game)
{
	int	i;
	int	j;
	int	count;

	i = 0;
	count = 0;
	while (i < game->map.height)
	{
		j = 0;
		while (game->map.grid[i][j])
		{
			if (game->map.grid[i][j] == 'D')
				count++;
			j++;
		}
		i++;
	}
	return (count);
}

void	ft_register_doors(t_game *game)
{
	int	i;
	int	j;
	int	index;

	i = 0;
	index = 0;
	while (i < game->map.height)
	{
		j = 0;
		while (game->map.grid[i][j])
		{
			if (game->map.grid[i][j] == 'D')
			{
				game->doors[index].x = j;
				game->doors[index].y = i;
				game->doors[index].is_open = 0;
				game->doors[index].orientation
					= ft_check_door_walls(game, i, j);
				index++;
			}
			j++;
		}
		i++;
	}
}
