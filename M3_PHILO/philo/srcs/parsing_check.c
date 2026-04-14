/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_check.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 14:56:30 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/14 11:42:14 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int	ft_check_is_digit(char *str)
{
	int	i;

	i = 0;
	if (!str[i])
		return (STANDARD_ERROR);
	if (ft_strlen(str) > 10)
		return (STANDARD_ERROR);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (STANDARD_ERROR);
		i++;
	}
	return (SUCCESS);
}

int	ft_check_zero(long value)
{
	if (value == 0)
		return (STANDARD_ERROR);
	return (SUCCESS);
}

int	ft_check_overflow(long value)
{
	if (value > INT_MAX)
		return (STANDARD_ERROR);
	return (SUCCESS);
}
