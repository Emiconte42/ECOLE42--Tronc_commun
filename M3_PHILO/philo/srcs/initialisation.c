/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialisation.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 09:38:14 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/15 14:42:23 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

static int	ft_init_forks(t_table *table)
{
	table->forks = calloc(table->number_of_philosophers,
			sizeof(pthread_mutex_t));
	if (!table->forks)
		return (STANDARD_ERROR);
	return (SUCCESS);
}

static int	ft_init_mutex(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->number_of_philosophers)
	{
		if (pthread_mutex_init(&table->forks[i], NULL) != 0)
		{
			while (i > 0)
			{
				pthread_mutex_destroy(&table->forks[i - 1]);
				i --;
			}
			return (STANDARD_ERROR);
		}	
		i++;
	}
	return (SUCCESS);
}

static int	ft_init_global_mutex(t_table *table)
{
	if (pthread_mutex_init(&table->print_mutex, NULL) != 0)
		return (STANDARD_ERROR);
	if (pthread_mutex_init(&table->death_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&table->print_mutex);
		return (STANDARD_ERROR);
	}
	if (pthread_mutex_init(&table->last_meal_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&table->print_mutex);
		pthread_mutex_destroy(&table->death_mutex);
		return (STANDARD_ERROR);
	}
	if (pthread_mutex_init(&table->full_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&table->last_meal_mutex);
		pthread_mutex_destroy(&table->death_mutex);
		pthread_mutex_destroy(&table->print_mutex);
		return (STANDARD_ERROR);
	}
	return (SUCCESS);
}

static int	ft_init_philos(t_table *table)
{
	int	i;

	i = 0;
	table->philos = calloc(table->number_of_philosophers, sizeof(t_philo));
	if (!table->philos)
		return (STANDARD_ERROR);
	while (i < table->number_of_philosophers)
	{
		table->philos[i].id = i + 1;
		table->philos[i].last_meal = 0;
		table->philos[i].fork_left = &table->forks[i];
		table->philos[i].fork_right = &table->forks[(i + 1)
			% table->number_of_philosophers];
		table->philos[i].table = table;
		i++;
	}
	return (SUCCESS);
}

int	ft_init(t_table *table)
{
	table->start_time = 0;
	if (ft_init_forks(table) != SUCCESS)
		return (STANDARD_ERROR);
	if (ft_init_mutex(table) != SUCCESS)
		return (ft_cleanup_init(table), STANDARD_ERROR);
	if (ft_init_global_mutex(table) != SUCCESS)
		return (ft_cleanup_init(table), STANDARD_ERROR);
	if (ft_init_philos(table) != SUCCESS)
		return (ft_cleanup_init(table), STANDARD_ERROR);
	return (SUCCESS);
}
