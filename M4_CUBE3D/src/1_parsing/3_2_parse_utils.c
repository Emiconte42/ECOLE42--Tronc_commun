/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   3_2_parse_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 12:03:09 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/17 15:42:27 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static int	ft_paths_equal(char *a, char *b)
{
	int	len_a;
	int	len_b;
	int	cmp;

	len_a = ft_strlen(a);
	len_b = ft_strlen(b);
	if (len_a != len_b)
		return (0);
	cmp = ft_strncmp(a, b, len_a + 1);
	if (cmp == 0)
		return (0);
	return (1);
}

int	ft_check_duplicate_textures(t_game *game)
{
	char	*tex_arr[4];
	int		i;
	int		j;

	tex_arr[0] = game->tex.no_path;
	tex_arr[1] = game->tex.so_path;
	tex_arr[2] = game->tex.we_path;
	tex_arr[3] = game->tex.ea_path;
	i = 0;
	while (i < 4)
	{
		j = i + 1;
		while (j < 4)
		{
			if (ft_paths_equal(tex_arr[i], tex_arr[j]))
			{
				printf("Error\nTwo textures use the same file\n");
				return (0);
			}
			j++;
		}
		i++;
	}
	return (1);
}

int	ft_check_duplicate_colors(t_game *game)
{
	if (game->floor.hex_color == game->ceiling.hex_color)
	{
		printf("Error\nFloor and ceiling colors are identical\n");
		return (0);
	}
	return (1);
}

int	ft_check_xpm_extension(char *path)
{
	int	len;

	len = ft_strlen(path);
	if (len < 5)
	{
		printf("Error\nTexture must be a '.xpm' file\n");
		return (0);
	}
	else if (ft_strncmp(&path[len - 4], ".xpm", 4) != 0)
	{
		printf("Error\nTexture must be a '.xpm' file\n");
		return (0);
	}
	return (1);
}
