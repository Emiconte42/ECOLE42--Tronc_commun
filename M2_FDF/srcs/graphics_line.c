/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   graphics_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 13:00:23 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/11 20:58:48 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

/*****************************************************************************
				DRAW A LINE BETWEEN TWO POINTS USING DDA
 *****************************************************************************/

void	draw_line(t_fdf *fdf, t_point a, t_point b, int color)
{
	t_dda	dda;
	int		steps;
	int		i;

	if (abs(b.x - a.x) > abs(b.y - a.y))
		steps = abs(b.x - a.x);
	else
		steps = abs(b.y - a.y);
	if (steps == 0)
	{
		draw_pixel(fdf, a.x, a.y, color);
		return ;
	}
	dda.x = a.x;
	dda.y = a.y;
	dda.x_inc = (b.x - a.x) / (float)steps;
	dda.y_inc = (b.y - a.y) / (float)steps;
	i = 0;
	while (i <= steps)
	{
		draw_pixel(fdf, (int)dda.x, (int)dda.y, color);
		dda.x += dda.x_inc;
		dda.y += dda.y_inc;
		i++;
	}
}
