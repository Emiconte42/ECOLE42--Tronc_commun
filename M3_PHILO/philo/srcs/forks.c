/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 13:13:13 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 13:06:08 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int	ft_unlock_one_fork(pthread_mutex_t *fork)
{
	if (pthread_mutex_unlock(fork) != 0)
		return (STANDARD_ERROR);
	return (SUCCESS);
}

int	ft_unlock_both_forks(t_philo *philo)
{
	int	ret;

	ret = SUCCESS;
	if (pthread_mutex_unlock(philo->fork_left) != 0)
		ret = STANDARD_ERROR;
	if (pthread_mutex_unlock(philo->fork_right) != 0)
		ret = STANDARD_ERROR;
	return (ret);
}

static int	ft_philo_odd(t_philo *philo)
{
	pthread_mutex_lock(philo->fork_left);
	pthread_mutex_lock(philo->fork_right);
	ft_printf(philo->table, philo, STR_FORK);
	ft_printf(philo->table, philo, STR_FORK);
	return (SUCCESS);
}

static int	ft_philo_even(t_philo *philo)
{
	pthread_mutex_lock(philo->fork_right);
	pthread_mutex_lock(philo->fork_left);
	ft_printf(philo->table, philo, STR_FORK);
	ft_printf(philo->table, philo, STR_FORK);
	return (SUCCESS);
}

int	ft_take_forks(t_philo *philo)
{
	if (ft_get_is_dead(philo->table))
		return (STANDARD_ERROR);
	if (philo->id % 2 != 0)
		return (ft_philo_odd(philo));
	else
		return (ft_philo_even(philo));
}
