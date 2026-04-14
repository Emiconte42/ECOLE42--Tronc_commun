/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printf.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 16:44:45 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 13:04:59 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	ft_printf(t_table *table, t_philo *philo, char *message)
{
	long	timestamp;

	pthread_mutex_lock(&table->death_mutex);
	if (!table->is_dead)
	{
		pthread_mutex_lock(&table->print_mutex);
		timestamp = get_current_time() - table->start_time;
		printf("%ld %d %s\n", timestamp, philo->id, message);
		pthread_mutex_unlock(&table->print_mutex);
	}
	pthread_mutex_unlock(&table->death_mutex);
}
