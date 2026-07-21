/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   3_1_parse_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 20:27:38 by hbelleuv          #+#    #+#             */
/*   Updated: 2026/07/17 15:22:27 by hbelleuv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

int	ft_check_map_not_last(char *line)
{
	int	i;

	i = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	if (ft_strncmp(&line[i], "NO ", 3) == 0
		|| ft_strncmp(&line[i], "SO ", 3) == 0
		|| ft_strncmp(&line[i], "WE ", 3) == 0
		|| ft_strncmp(&line[i], "EA ", 3) == 0
		|| ft_strncmp(&line[i], "F ", 2) == 0
		|| ft_strncmp(&line[i], "C ", 2) == 0)
	{
		printf("Error\nMap not last\n");
		return (0);
	}
	return (1);
}

int	ft_parse_texture(t_game *game, char **target_path, char *line)
{
	int	i;
	int	len;
	int	fd;

	i = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
	*target_path = ft_gc_strdup(&line[i], &game->gc);
	if (ft_check_xpm_extension(*target_path) == 0)
		return (0);
	fd = open(*target_path, O_RDONLY);
	if (fd < 0)
	{
		printf("Error\nInvalid texture path\n");
		return (0);
	}
	close(fd);
	return (1);
}

static int	ft_parse_single_rgb(char *line, int *i, int *value, int *commas)
{
	if (!ft_isdigit(line[*i]))
	{
		printf("Error\nInvalid character\n");
		return (0);
	}
	*value = ft_atoi(&line[*i]);
	if (*value < 0 || *value > 255)
	{
		printf("Error\nRGB value out of range\n");
		return (0);
	}
	while (ft_isdigit(line[*i]))
		(*i)++;
	while (line[*i] == ' ' || line[*i] == '\t')
		(*i)++;
	if (line[*i] == ',')
	{
		(*commas)++;
		(*i)++;
		while (line[*i] == ' ' || line[*i] == '\t')
			(*i)++;
	}
	return (1);
}

int	ft_parse_color(t_game *game, t_rgb *color, char *line)
{
	int	i;
	int	commas;
	int	*rgb_arr[3];
	int	rgb_index;

	(void)game;
	i = 0;
	rgb_arr[0] = &color->r;
	rgb_arr[1] = &color->g;
	rgb_arr[2] = &color->b;
	rgb_index = 0;
	commas = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	while (line[i] && line[i] != '\n' && rgb_index < 3)
	{
		if (ft_parse_single_rgb(line, &i, rgb_arr[rgb_index], &commas) == 0)
			return (0);
		rgb_index++;
	}
	if (commas != 2 || rgb_index != 3 || (line[i] != '\n' && line[i] != '\0'))
		return (printf("Error\nInvalid RGB format (expected : 'R,G,B')\n"), 0);
	color->hex_color = (color->r << 16) | (color->g << 8) | color->b;
	return (1);
}

int	ft_check_extension(char *filename)
{
	int	len;

	len = ft_strlen(filename);
	if (len < 5)
		return (0);
	if (ft_strncmp(&filename[len - 4], ".cub", 4) == 0)
		return (1);
	return (0);
}
