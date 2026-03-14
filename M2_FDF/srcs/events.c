/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 16:52:38 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/18 12:54:11 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

/*****************************************************************************
					HANDLE KEYBOARD AND MOUSE EVENTS
*****************************************************************************/

/*===========================================
Redraw the image
===========================================*/
static void	redraw(t_fdf *fdf)
{
	ft_memset(
		fdf->img.img_addr,
		0,
		fdf->img.size_line * fdf->window.win_height);
	draw_map(fdf);
	mlx_put_image_to_window(
		fdf->window.mlx,
		fdf->window.window,
		fdf->img.img_ptr,
		0,
		0);
}

/*===========================================
Mouse
===========================================*/
int	mouse_hook(int button, int x, int y, void *param)
{
	t_fdf	*fdf;

	fdf = (t_fdf *)param;
	if (button == WHEEL_UP)
		zoom_at(fdf, x, y, 1);
	else if (button == WHEEL_DOWN)
		zoom_at(fdf, x, y, -1);
	else
		return (0);
	redraw(fdf);
	return (0);
}

/*===========================================
Keyboard
===========================================*/
static int	handle_key(int keycode, t_fdf *fdf)
{
	if (keycode == KEY_ZOOM_IN)
		zoom_in(fdf);
	else if (keycode == KEY_ZOOM_OUT)
		zoom_out(fdf);
	else if (keycode == KEY_SPACE)
		space_center(fdf);
	else if (keycode == KEY_A || keycode == KEY_LEFT)
		move_left(fdf);
	else if (keycode == KEY_D || keycode == KEY_RIGHT)
		move_right(fdf);
	else if (keycode == KEY_W || keycode == KEY_UP)
		move_up(fdf);
	else if (keycode == KEY_S || keycode == KEY_DOWN)
		move_down(fdf);
	else
		return (0);
	return (1);
}

int	key_hook(int keycode, void *param)
{
	t_fdf	*fdf;

	fdf = (t_fdf *)param;
	if (keycode == KEY_ESC)
	{
		clean_and_exit(fdf);
		return (0);
	}
	if (handle_key(keycode, fdf) == 0)
		return (0);
	redraw(fdf);
	return (0);
}
