/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   5_map_validate_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 18:10:52 by hbelleuv          #+#    #+#             */
/*   Updated: 2026/07/08 12:22:46 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static void	ft_fix_line_spaces(char *line)
{
	int	first;
	int	last;
	int	i;

	first = 0;
	while (line[first] == ' ')
		first++;
	last = ft_strlen(line) - 1;
	while (last > first && line[last] == ' ')
		last--;
	i = first;
	while (i < last)
	{
		if (line[i] == ' ')
			line[i] = '0';
		i++;
	}
}

void	ft_fix_map_spaces(t_game *game)
{
	int	i;

	i = 0;
	while (i < game->map.height)
	{
		ft_fix_line_spaces(game->map.grid[i]);
		i++;
	}
}

int	ft_check_cell(t_game *game, int i, int j)
{
	char	c;

	c = game->map.grid[i][j];
	if (c == '0' || c == 'N' || c == 'S' || c == 'W' || c == 'E')
		return (ft_check_surrounding(game, i, j));
	if (c == 'D' && ft_check_door_walls(game, i, j) == DOOR_INVALID)
		return (0);
	return (1);
}
