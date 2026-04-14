/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 13:54:24 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 11:42:55 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

static void	init_simulation(t_table *table)
{
	int	i;

	table->start_time = get_current_time();
	i = 0;
	while (i < table->number_of_philosophers)
	{
		table->philos[i].last_meal = table->start_time;
		i++;
	}
}

int	ft_start(t_table *table)
{
	init_simulation(table);
	if (handle_one_philo(table) == SUCCESS)
		return (SUCCESS);
	if (create_threads(table))
		return (STANDARD_ERROR);
	join_threads(table);
	return (SUCCESS);
}
