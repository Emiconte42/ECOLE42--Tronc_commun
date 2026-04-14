/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 15:29:32 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 13:02:14 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

static int	ft_eat(t_philo *philo)
{
	if (ft_take_forks(philo) != SUCCESS)
		return (STANDARD_ERROR);
	ft_set_last_meal(philo);
	ft_printf(philo->table, philo, STR_EAT);
	philo->status = EATING;
	ft_wait(philo);
	pthread_mutex_unlock(philo->fork_left);
	pthread_mutex_unlock(philo->fork_right);
	if (ft_check_satiated(philo) != SUCCESS)
		return (STANDARD_ERROR);
	return (SUCCESS);
}

static int	ft_sleep(t_philo *philo)
{
	if (ft_get_is_dead(philo->table))
		return (STANDARD_ERROR);
	ft_printf(philo->table, philo, STR_SLEEP);
	philo->status = SLEEPING;
	ft_wait(philo);
	if (ft_get_is_dead(philo->table))
		return (STANDARD_ERROR);
	return (SUCCESS);
}

static int	ft_think(t_philo *philo)
{
	if (ft_get_is_dead(philo->table))
		return (STANDARD_ERROR);
	ft_printf(philo->table, philo, STR_THINK);
	philo->status = THINKING;
	ft_wait(philo);
	if (ft_get_is_dead(philo->table))
		return (STANDARD_ERROR);
	return (SUCCESS);
}

void	*ft_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->id % 2 == 0)
		usleep((philo->table->time_to_eat / 2) * 1000);
	while (1)
	{
		if (ft_eat(philo) != SUCCESS)
			return (NULL);
		if (ft_sleep(philo) != SUCCESS)
			return (NULL);
		if (ft_think(philo) != SUCCESS)
			return (NULL);
	}
	return (NULL);
}
