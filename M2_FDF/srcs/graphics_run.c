/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   graphics_run.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 14:54:24 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/03 18:52:45 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

/*****************************************************************************
					ORCHESTRATE THE GRAPHICAL EXECUTION
	INITIALIZE GRAPHICS, HANDLE EVENTS, DRAW THE MAP AND START THE MLX LOOP
*****************************************************************************/

/*=====================================================
Create an off-screen image used for drawing
=====================================================*/
static int	init_img(t_fdf *fdf)
{
	fdf->img.img_ptr = mlx_new_image(
			fdf->window.mlx,
			fdf->window.win_width,
			fdf->window.win_height);
	if (!fdf->img.img_ptr)
		return (0);
	fdf->img.img_addr = mlx_get_data_addr(
			fdf->img.img_ptr,
			&fdf->img.bits_per_pixels,
			&fdf->img.size_line,
			&fdf->img.endian);
	return (1);
}

/*=====================================================
Calcul origin x and y for the start
=====================================================*/
static void	compute_origin(t_fdf *fdf)
{
	fdf->view.origin_x = (fdf->window.win_width
			- ((fdf->map.width + fdf->map.height - 2)
				* fdf->view.spacing)) / 2
		+ (fdf->map.height - 1)
		* fdf->view.spacing;
	fdf->view.origin_y = (fdf->window.win_height
			- ((fdf->map.width + fdf->map.height - 2)
				* fdf->view.spacing / 2)) / 2;
}

/*=====================================================
Main graphical loop: initialize, draw and handle events
=====================================================*/
int	graphics_run(t_fdf *fdf)
{
	if (init_mlx(fdf) == 0)
		return (0);
	if (init_img(fdf) == 0)
		return (0);
	compute_origin(fdf);
	draw_map(fdf);
	mlx_put_image_to_window(
		fdf->window.mlx,
		fdf->window.window,
		fdf->img.img_ptr,
		0,
		0);
	mlx_key_hook(fdf->window.window, key_hook, fdf);
	mlx_mouse_hook(fdf->window.window, mouse_hook, fdf);
	mlx_hook(fdf->window.window, 17, 0, clean_and_exit, fdf);
	mlx_loop(fdf->window.mlx);
	return (1);
}
