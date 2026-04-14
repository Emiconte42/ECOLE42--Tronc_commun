/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   meals.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 11:37:48 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 12:08:07 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int	ft_check_satiated(t_philo *philo)
{
	pthread_mutex_lock(&philo->table->last_meal_mutex);
	philo->meal_count++;
	if (philo->table->must_eat_count != -1
		&& philo->meal_count >= philo->table->must_eat_count)
	{
		pthread_mutex_unlock(&philo->table->last_meal_mutex);
		pthread_mutex_lock(&philo->table->full_mutex);
		philo->table->full++;
		pthread_mutex_unlock(&philo->table->full_mutex);
		return (STANDARD_ERROR);
	}
	pthread_mutex_unlock(&philo->table->last_meal_mutex);
	return (SUCCESS);
}

long	ft_get_meal_count(t_philo *philo)
{
	long	value;

	value = 0;
	pthread_mutex_lock(&philo->table->last_meal_mutex);
	value = philo->meal_count;
	pthread_mutex_unlock(&philo->table->last_meal_mutex);
	return (value);
}

void	ft_set_last_meal(t_philo *philo)
{
	pthread_mutex_lock(&philo->table->last_meal_mutex);
	philo->last_meal = get_current_time();
	pthread_mutex_unlock(&philo->table->last_meal_mutex);
}

long	ft_get_last_meal(t_philo *philo)
{
	long	value;

	value = 0;
	pthread_mutex_lock(&philo->table->last_meal_mutex);
	value = philo->last_meal;
	pthread_mutex_unlock(&philo->table->last_meal_mutex);
	return (value);
}
