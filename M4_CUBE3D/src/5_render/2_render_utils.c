/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_render_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 13:51:57 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/07 14:46:36 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"

static int	ft_is_transparent(int color)
{
	int	r;
	int	g;
	int	b;

	r = (color >> 16) & 0xFF;
	g = (color >> 8) & 0xFF;
	b = color & 0xFF;
	return (r >= 50 && r <= 64
		&& g >= 50 && g <= 64
		&& b >= 50 && b <= 64);
}

static void	ft_draw_weapon_img(t_game *game, t_img *img, int x_start,
		int y_start)
{
	int	x;
	int	y;
	int	color;

	y = 0;
	while (y < img->height)
	{
		x = 0;
		while (x < img->width)
		{
			color = ft_get_pixel_from_tex(img, x, y);
			if (!ft_is_transparent(color))
				put_pixel(game, x_start + x, y_start + y, color);
			x++;
		}
		y++;
	}
}

void	ft_draw_weapon(t_game *game)
{
	t_img	*img;
	int		x_start;
	int		y_start;

	if (game->keys.is_shooting)
		img = &game->tex.waepon_shoot;
	else
		img = &game->tex.waepon;
	x_start = WINDOW_WIDTH / 2 - img->width / 2;
	y_start = WINDOW_HEIGHT - img->height;
	ft_draw_weapon_img(game, img, x_start, y_start);
}
