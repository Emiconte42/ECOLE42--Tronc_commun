/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialization.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 12:34:55 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/18 12:53:34 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

/*****************************************************************************
					THIS IS INITIALIZATION FOR FDF	
*****************************************************************************/

/*===========================================
Initial parameter of view
===========================================*/
void	init_view(t_fdf *fdf)
{
	int	spacing_x;
	int	spacing_y;

	fdf->window.win_width = 1920;
	fdf->window.win_height = 1080;
	spacing_x = fdf->window.win_width / (fdf->map.width + 2);
	spacing_y = fdf->window.win_height / (fdf->map.height + 2);
	if (spacing_x < spacing_y)
		fdf->view.spacing = spacing_x;
	else
		fdf->view.spacing = spacing_y;
	if (fdf->view.spacing < 2)
		fdf->view.spacing = 2;
}

/*===========================================
Configuration initial
===========================================*/
void	init_config(t_fdf *fdf)
{
	fdf->window.mlx = NULL;
	fdf->window.window = NULL;
}

/*=====================================================
Create the MLX context and the main window
=====================================================*/
int	init_mlx(t_fdf *fdf)
{
	fdf->window.mlx = mlx_init();
	if (!fdf->window.mlx)
		return (0);
	fdf->window.window = mlx_new_window(
			fdf->window.mlx,
			fdf->window.win_width,
			fdf->window.win_height,
			fdf->map.filename);
	if (!fdf->window.window)
		return (0);
	return (1);
}

/*===========================================
Calcul the min and the max for the color
===========================================*/
void	find_min_max(t_fdf *fdf)
{
	int	x;
	int	y;

	fdf->map.z_min = fdf->map.data[0][0];
	fdf->map.z_max = fdf->map.data[0][0];
	y = 0;
	while (y < fdf->map.height)
	{
		x = 0;
		while (x < fdf->map.width)
		{
			if (fdf->map.data[y][x] < fdf->map.z_min)
				fdf->map.z_min = fdf->map.data[y][x];
			if (fdf->map.data[y][x] > fdf->map.z_max)
				fdf->map.z_max = fdf->map.data[y][x];
			x++;
		}
		y++;
	}
}

/*===========================================
Name filename without "/"
===========================================*/
char	*get_filename(char *argv)
{
	int	i;

	i = 0;
	while (argv[i])
		i++;
	while (i > 0 && argv[i] != '/')
		i--;
	if (i > 0 && argv[i] == '/')
		i++;
	return (argv + i);
}
