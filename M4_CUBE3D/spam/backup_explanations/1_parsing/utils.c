/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbelleuv </var/spool/mail/hbelleuv>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 20:27:38 by hbelleuv          #+#    #+#             */
/*   Updated: 2026/06/28 14:28:27 by hbelleuv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"
#include <stdio.h>
#include <unistd.h>

int	ft_parse_texture(t_game *game, char **target_path, char *line)
{
	int	i;
	int	len;
	int	fd;

	i = 0;
	// ignorer les espaces ou tab
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	// retirer le \n a la fin de la ligne
	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
	// dupliquer la chaine a partir du vrai caractere
	// utilisation du ptr target_path pour modif la struc dans t_game
	*target_path = ft_gc_strdup(&line[i], &game->gc);
	fd = open(*target_path, O_RDONLY);
	if (fd < 0)
	{
		printf("Error : invalid texture path\n");
		return (0);
	}
	close(fd);
	return (1);
}

int	ft_parse_color(t_game *game, t_rgd *color, char *line)
{
	int	i;
	int	commas;
	int	*rgb_arr[3];
	int	rgb_index;

	(void)game;
	i = 0;
	rgb_arr[0] = &color->r;
	rgb_arr[1] = &color->g;
	rgb_arr[2] = &color->b;
	rgb_index = 0;
	commas = 0;
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	while (line[i] && line[i] != '\n' && rgb_index < 3)
	{
		if (!ft_isdigit(line[i]))
		{
			printf("Error : invalid character\n");
			return (0);
		}
		*rgb_arr[rgb_index] = ft_atoi(&line[i]);
		if (*rgb_arr[rgb_index] < 0 || *rgb_arr[rgb_index] > 255)
		{
			printf("Error : RGB value out of range\n");
			return (0);
		}
		while (ft_isdigit(line[i]))
			i++;
		while (line[i] == ' ' || line[i] == '\t')
			i++;
		if (line[i] == ',')
		{
			commas++;
			i++;
			while (line[i] == ' ' || line[i] == '\t')
				i++;
		}
		rgb_index++;
	}
	if (commas != 2 || rgb_index != 3 || (line[i] != '\n' && line[i] != '\0'))
	{
		printf("Error : invalid RGB format (expected : 'R,G,B')\n");
		return (0);
	}
	return (1);
}
