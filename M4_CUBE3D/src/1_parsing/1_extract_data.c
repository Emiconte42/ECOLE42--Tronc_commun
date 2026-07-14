/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   1_extract_data.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 11:22:41 by hbelleuv          #+#    #+#             */
/*   Updated: 2026/07/14 12:11:17 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static int	ft_is_begin_map(char *line)
{
	int	i;

	i = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	if (line[i] == '\0' || line[i] == '\n')
		return (0);
	if (line[i] == '0' || line[i] == '1')
		return (1);
	return (0);
}

static char	**ft_alloc_copy_grid(t_game *game)
{
	char	**new_grid;
	int		i;

	new_grid = ft_gc_calloc(game->map.height + 2, sizeof(char *), &game->gc);
	if (!new_grid)
	{
		printf("Error : incorrect map allocation\n");
		ft_close_window(game);
		exit(1);
	}
	i = 0;
	while (i < game->map.height)
	{
		new_grid[i] = game->map.grid[i];
		i++;
	}
	return (new_grid);
}

static int	ft_append_line_to_map(t_game *game, char *line)
{
	char	**new_grid;
	int		len;

	if (ft_check_map_not_last(line) == 0)
		return (0);
	new_grid = ft_alloc_copy_grid(game);
	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
	new_grid[game->map.height] = ft_gc_strdup(line, &game->gc);
	new_grid[game->map.height + 1] = NULL;
	game->map.grid = new_grid;
	game->map.height++;
	len = ft_strlen(line);
	if (len > game->map.width)
		game->map.width = len;
	return (1);
}

static int	ft_process_file_lines(t_game *game, int fd)
{
	char	*line;
	int		is_map;

	is_map = 0;
	line = ft_gc_get_next_line(&game->gc, fd);
	while (line != NULL)
	{
		if (is_map == 0 && ft_is_begin_map(line))
			is_map = 1;
		if (is_map == 1)
		{
			if (ft_append_line_to_map(game, line) == 0)
				return (0);
		}
		else if (ft_parse_param(game, line) == 0)
			return (0);
		line = ft_gc_get_next_line(&game->gc, fd);
	}
	return (1);
}

int	ft_parse_cub_file(char *filename, t_game *game)
{
	int		fd;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return (perror("open"), 0);
	if (ft_process_file_lines(game, fd) == 0)
		return (close(fd), 0);
	if (ft_check_missing_elements(game) == 0)
		return (close(fd), 0);
	close(fd);
	game->doors_count = ft_count_doors(game);
	if (game->doors_count > 0)
	{
		game->doors = ft_gc_calloc(game->doors_count,
				sizeof(t_door), &game->gc);
		if (!game->doors)
			return (ERROR);
		ft_register_doors(game);
	}
	return (1);
}
