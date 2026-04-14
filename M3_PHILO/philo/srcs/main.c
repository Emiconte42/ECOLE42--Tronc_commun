/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 13:50:35 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 19:12:40 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int	main(int argc, char **argv)
{
	t_table	*table;
	int		result;

	result = SUCCESS;
	table = calloc(1, sizeof(t_table));
	if (!table)
		return (STANDARD_ERROR);
	if (ft_parsing(table, argc, argv) != SUCCESS)
		return (printf(ERR), ft_cleanup(table), STANDARD_ERROR);
	else if (ft_init(table) != SUCCESS)
		return (ft_cleanup_init(table), STANDARD_ERROR);
	else if (ft_start(table) != SUCCESS)
		return (ft_cleanup_init(table), STANDARD_ERROR);
	ft_cleanup(table);
	return (result);
}
