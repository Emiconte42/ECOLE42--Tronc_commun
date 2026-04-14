/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 14:56:08 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 19:13:13 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

static int	ft_check_death(t_table *table)
{
	int		i;
	long	time_since_meal;

	i = 0;
	while (i < table->number_of_philosophers)
	{
		time_since_meal = get_current_time()
			- ft_get_last_meal(&table->philos[i]);
		if (time_since_meal > table->time_to_die)
		{
			ft_set_is_dead(table);
			pthread_mutex_lock(&table->print_mutex);
			printf("%ld %d %s\n", get_current_time()
				- table->start_time, table->philos[i].id, STR_DEAD);
			pthread_mutex_unlock(&table->print_mutex);
			return (STANDARD_ERROR);
		}
		i++;
	}
	return (SUCCESS);
}

void	*ft_monitor(void *arg)
{
	t_table	*table;

	table = (t_table *)arg;
	while (1)
	{
		if (ft_check_death(table))
			return (NULL);
		pthread_mutex_lock(&table->full_mutex);
		if (table->must_eat_count != -1 && table->full
			== table->number_of_philosophers)
		{
			pthread_mutex_unlock(&table->full_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&table->full_mutex);
		usleep(100);
	}
	return (NULL);
}
