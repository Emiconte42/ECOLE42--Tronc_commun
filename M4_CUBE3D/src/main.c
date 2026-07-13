/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 14:02:41 by emiconte          #+#    #+#             */
/*   Updated: 2026/07/08 11:42:33 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

static int	ft_check_args(int argc, char **argv, t_game *game)
{
	if (argc != 2)
	{
		printf("Error : Usage : './cub3d <carte.cub>'\n");
		return (0);
	}
	if (ft_check_extension(argv[1]) == 0)
	{
		printf("Error : you must submit a .cub file\n");
		ft_close_window(game);
		exit(1);
	}
	if (ft_parse_cub_file(argv[1], game) == 0)
	{
		ft_close_window(game);
		exit(1);
	}
	return (1);
}

static int	ft_init_and_run(t_game *game)
{
	if (ft_init_window(game) != SUCCESS)
		return (printf("Error : init window\n"), ERROR);
	if (ft_init_img(game) != SUCCESS)
	{
		ft_close_window(game);
		return (printf("Error : init img\n"), ERROR);
	}
	if (ft_run_window(game) != SUCCESS)
	{
		ft_close_window(game);
		return (printf("Error : run window\n"), ERROR);
	}
	return (SUCCESS);
}

int	main(int argc, char **argv)
{
	t_game	game;

	ft_memset(&game, 0, sizeof(t_game));
	if (ft_check_args(argc, argv, &game) == 0)
		return (1);
	if (ft_init_and_run(&game) != SUCCESS)
		return (ERROR);
	ft_close_window(&game);
	return (0);
}
