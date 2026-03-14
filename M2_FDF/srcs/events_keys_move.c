/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events_keys_move.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 12:32:11 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/12 13:06:12 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

/*****************************************************************************
								KEYBOARD MOVE EVENTS	
*****************************************************************************/

void	move_left(t_fdf *fdf)
{
	fdf->view.origin_x += 10;
}

void	move_right(t_fdf *fdf)
{
	fdf->view.origin_x -= 10;
}

void	move_up(t_fdf *fdf)
{
	fdf->view.origin_y += 10;
}

void	move_down(t_fdf *fdf)
{
	fdf->view.origin_y -= 10;
}

void	space_center(t_fdf *fdf)
{
	int	proj_width;
	int	proj_height;

	proj_width = (
			fdf->map.width + fdf->map.height - 2) * fdf->view.spacing;
	proj_height = (
			fdf->map.width + fdf->map.height - 2) * fdf->view.spacing / 2;
	fdf->view.origin_x = (fdf->window.win_width - proj_width) / 2
		+ (fdf->map.height - 1) * fdf->view.spacing;
	fdf->view.origin_y = (fdf->window.win_height - proj_height) / 2;
}
