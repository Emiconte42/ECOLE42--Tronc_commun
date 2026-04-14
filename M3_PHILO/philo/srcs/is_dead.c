/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_dead.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 11:41:13 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 13:07:17 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int	ft_is_dead_only(t_table *table)
{
	int	value;

	pthread_mutex_lock(&table->death_mutex);
	value = table->is_dead;
	pthread_mutex_unlock(&table->death_mutex);
	if (value)
		return (STANDARD_ERROR);
	return (SUCCESS);
}

void	ft_set_is_dead(t_table *table)
{
	pthread_mutex_lock(&table->death_mutex);
	table->is_dead = 1;
	pthread_mutex_unlock(&table->death_mutex);
}

int	ft_get_is_dead(t_table *table)
{
	int	value;

	pthread_mutex_lock(&table->death_mutex);
	value = table->is_dead;
	pthread_mutex_unlock(&table->death_mutex);
	if (value)
		return (STANDARD_ERROR);
	pthread_mutex_lock(&table->full_mutex);
	if (table->must_eat_count != -1
		&& table->full == table->number_of_philosophers)
	{
		pthread_mutex_unlock(&table->full_mutex);
		return (STANDARD_ERROR);
	}
	pthread_mutex_unlock(&table->full_mutex);
	return (SUCCESS);
}
