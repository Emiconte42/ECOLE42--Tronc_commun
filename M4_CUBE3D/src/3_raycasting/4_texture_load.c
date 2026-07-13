/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   4_texture_load.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 15:22:30 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/08 12:25:10 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static void	ft_load_single_tex(t_game *game, t_img *tex, char *path)
{
	tex->img_ptr = mlx_xpm_file_to_image(game->window.mlx, path,
			&tex->width, &tex->height);
	if (tex->img_ptr == NULL)
	{
		printf("Error : failed to load texture %s\n", path);
		ft_close_window(game);
		exit(1);
	}
	tex->img_addr = mlx_get_data_addr(tex->img_ptr, &tex->bits_per_pixels,
			&tex->size_line, &tex->endian);
}

static void	ft_load_door_tex(t_game *game)
{
	int		i;
	char	*num;
	char	*path;

	i = 0;
	while (i < DOOR_ANIM_FRAMES)
	{
		num = ft_gc_itoa(i, &game->gc);
		path = ft_gc_strjoin(DOOR_TEX_PATH, num, &game->gc);
		path = ft_gc_strjoin(path, DOOR_TEX_SUFFIX, &game->gc);
		ft_load_single_tex(game, &game->tex.door[i], path);
		i++;
	}
}

void	ft_load_all_tex(t_game *game)
{
	ft_load_single_tex(game, &game->tex.no, game->tex.no_path);
	ft_load_single_tex(game, &game->tex.so, game->tex.so_path);
	ft_load_single_tex(game, &game->tex.we, game->tex.we_path);
	ft_load_single_tex(game, &game->tex.ea, game->tex.ea_path);
	ft_load_single_tex(game, &game->tex.waepon, WAEPON);
	ft_load_single_tex(game, &game->tex.waepon_shoot, WAEPON_SHOOT);
	ft_load_door_tex(game);
}
