/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events_keys_zoom.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 12:32:11 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/15 18:47:06 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

/*****************************************************************************
								ZOOM EVENTS
*****************************************************************************/

void	zoom_in(t_fdf *fdf)
{
	fdf->view.spacing += 1;
	fdf->view.origin_x -= fdf->map.width / 2;
	fdf->view.origin_y -= fdf->map.height / 2;
}

void	zoom_out(t_fdf *fdf)
{
	if (fdf->view.spacing > 1)
	{
		fdf->view.spacing -= 1;
		fdf->view.origin_x += fdf->map.width / 2;
		fdf->view.origin_y += fdf->map.height / 2;
	}
}

void	zoom_at(t_fdf *fdf, int mouse_x, int mouse_y, int delta)
{
	int		old_s;
	int		new_s;
	double	ratio;

	old_s = fdf->view.spacing;
	new_s = old_s + delta;
	if (new_s < 1)
		return ;
	ratio = (double)new_s / (double)old_s;
	fdf->view.origin_x = mouse_x - (int)((mouse_x
				- fdf->view.origin_x) * ratio);
	fdf->view.origin_y = mouse_y - (int)((mouse_y
				- fdf->view.origin_y) * ratio);
	fdf->view.spacing = new_s;
}
