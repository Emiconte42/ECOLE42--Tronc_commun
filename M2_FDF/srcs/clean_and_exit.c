/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_and_exit.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 16:51:19 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/18 12:54:55 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

/*****************************************************************************
				HANDLE CLEAN EXIT AND RESOURCE DESTRUCTION 
******************************************************************************/

/*===========================================
Clean and exit
===========================================*/
int	clean_and_exit(t_fdf *fdf)
{
	if (!fdf)
		exit(0);
	if (fdf->window.mlx && fdf->img.img_ptr)
		mlx_destroy_image(fdf->window.mlx, fdf->img.img_ptr);
	if (fdf->window.mlx && fdf->window.window)
		mlx_destroy_window(fdf->window.mlx, fdf->window.window);
	if (fdf->window.mlx)
	{
		mlx_destroy_display(fdf->window.mlx);
		free(fdf->window.mlx);
	}
	map_free(fdf);
	exit(0);
}
