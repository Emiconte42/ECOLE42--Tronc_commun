/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 10:41:11 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/18 19:15:29 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

/*****************************************************************************
		THE MAIN FUNCTION IS THE CONDUCTOR OF THE WHOLE FDF PROJECT. 
******************************************************************************/

int	main(int argc, char **argv)
{
	t_fdf	fdf;

	if (argc != 2)
	{
		ft_putendl_fd("Error", 2);
		return (1);
	}
	ft_bzero(&fdf, sizeof(t_fdf));
	if (!map_load(&fdf, argv[1]))
	{
		get_next_line(-1);
		return (1);
	}
	init_config(&fdf);
	init_view(&fdf);
	fdf.map.filename = get_filename(argv[1]);
	graphics_run(&fdf);
	clean_and_exit(&fdf);
	return (0);
}
