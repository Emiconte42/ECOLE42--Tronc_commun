/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   1philo.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 13:21:27 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 13:04:38 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int	handle_one_philo(t_table *table)
{
	if (table->number_of_philosophers == 1)
	{
		ft_printf(table, &table->philos[0], STR_FORK);
		usleep(table->time_to_die * 1000);
		ft_printf(table, &table->philos[0], STR_DEAD);
		return (SUCCESS);
	}
	return (STANDARD_ERROR);
}
