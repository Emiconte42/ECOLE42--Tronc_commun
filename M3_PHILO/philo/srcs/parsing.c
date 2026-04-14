/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 16:44:52 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 18:42:08 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

static int	ft_check_arguments(int argc)
{
	if (argc < 5 || argc > 6)
		return (STANDARD_ERROR);
	return (SUCCESS);
}

static int	ft_argc_5(t_table *table, char **argv)
{
	long	value;
	int		i;

	i = 1;
	while (i < 5)
	{
		if (ft_check_is_digit(argv[i]) != SUCCESS)
			return (STANDARD_ERROR);
		value = ft_atol(argv[i]);
		if (ft_check_zero(value) != SUCCESS
			|| ft_check_overflow(value) != SUCCESS
			|| value < 0 || value > INT_MAX)
			return (STANDARD_ERROR);
		if (i == 1)
			table->number_of_philosophers = value;
		else if (i == 2)
			table->time_to_die = value;
		else if (i == 3)
			table->time_to_eat = value;
		else if (i == 4)
			table->time_to_sleep = value;
		i++;
	}
	return (SUCCESS);
}

static int	ft_argc_6(t_table *table, int argc, char **argv)
{
	long	value;

	if (argc == 6)
	{
		if (ft_check_is_digit(argv[5]) != SUCCESS)
			return (STANDARD_ERROR);
		value = ft_atol(argv[5]);
		if (ft_check_zero(value) != SUCCESS
			|| ft_check_overflow(value) != SUCCESS
			|| value < 0 || value > INT_MAX)
			return (STANDARD_ERROR);
		table->must_eat_count = value;
	}
	else
		table->must_eat_count = -1;
	return (SUCCESS);
}

int	ft_parsing(t_table *table, int argc, char **argv)
{
	if (ft_check_arguments(argc) != SUCCESS)
		return (STANDARD_ERROR);
	if (ft_argc_5(table, argv) != SUCCESS)
		return (STANDARD_ERROR);
	if (ft_argc_6(table, argc, argv) != SUCCESS)
		return (STANDARD_ERROR);
	return (SUCCESS);
}
