/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 11:56:27 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 19:13:03 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	ft_cleanup(t_table *table)
{
	if (!table)
		return ;
	if (table && table->forks)
		free(table->forks);
	if (table && table->philos)
		free(table->philos);
	if (table)
		free(table);
}

void	ft_cleanup_init(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->number_of_philosophers)
	{
		if (&table->forks[i])
			pthread_mutex_destroy(&table->forks[i]);
		i++;
	}
	pthread_mutex_destroy(&table->print_mutex);
	pthread_mutex_destroy(&table->death_mutex);
	pthread_mutex_destroy(&table->last_meal_mutex);
	pthread_mutex_destroy(&table->full_mutex);
	if (table->forks)
		free(table->forks);
	if (table->philos)
		free(table->philos);
}
