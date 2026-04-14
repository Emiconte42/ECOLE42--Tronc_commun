/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 16:41:12 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 18:51:15 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

long	get_current_time(void)
{
	struct timeval	current_time;

	if (gettimeofday(&current_time, NULL) != 0)
		return (-1);
	return ((current_time.tv_sec * 1000) + (current_time.tv_usec / 1000));
}

void	ft_wait(t_philo *philo)
{
	long	start;
	long	duration;

	start = get_current_time();
	if (philo->status == EATING)
		duration = philo->table->time_to_eat;
	else if (philo->status == SLEEPING)
		duration = philo->table->time_to_sleep;
	else
		duration = 1;
	if (duration < 0)
		duration = 0;
	while (!ft_get_is_dead(philo->table))
	{
		if (get_current_time() - start >= duration)
			break ;
		usleep(500);
	}
}
