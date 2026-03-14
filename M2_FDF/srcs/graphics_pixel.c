/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   graphics_pixel.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 16:25:36 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/11 20:52:54 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

/*****************************************************************************
									PUT A PIXEL 
******************************************************************************/

void	draw_pixel(t_fdf *fdf, int x, int y, int color)
{
	char	*dest;

	if (x < 0 || y < 0 || x >= fdf->window.win_width
		|| y >= fdf->window.win_height)
		return ;
	dest = fdf->img.img_addr + (y * fdf->img.size_line + x
			* (fdf->img.bits_per_pixels / 8));
	*(unsigned int *)dest = color;
}
