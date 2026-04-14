/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 14:56:14 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 12:55:55 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	join_threads(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->number_of_philosophers)
	{
		if (pthread_join(table->philos[i].thread, NULL) != 0)
			break ;
		i++;
	}
	if (pthread_join(table->monitor, NULL) != 0)
		return ;
}

int	create_threads(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->number_of_philosophers)
	{
		if (pthread_create(&table->philos[i].thread,
				NULL, ft_routine, &table->philos[i]) != 0)
			return (1);
		i++;
	}
	if (pthread_create(&table->monitor, NULL, ft_monitor, table) != 0)
		return (1);
	return (0);
}
