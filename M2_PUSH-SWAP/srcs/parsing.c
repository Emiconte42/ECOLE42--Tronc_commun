/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 11:18:40 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/13 13:23:29 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pushswap.h"

static int	parse_validate(char *str)
{
	int		i;
	long	nb;

	i = 0;
	if (str[0] == '\0')
		return (1);
	if (str[i] == '+' || str[i] == '-')
	{
		i++;
		if (str[i] == '\0')
			return (1);
	}
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (1);
		i++;
	}
	nb = ft_atol(str);
	if (nb < INT_MIN || nb > INT_MAX)
		return (1);
	return (0);
}

static int	parse_count(char **argv)
{
	int		i;
	int		j;
	int		count;
	char	**split;

	i = 1;
	count = 0;
	while (argv[i])
	{
		split = ft_split(argv[i], ' ');
		if (!split)
			return (-1);
		j = 0;
		while (split[j])
		{
			count++;
			j++;
		}
		ft_free(split);
		i++;
	}
	return (count);
}

static int	parse_fill(char **argv, long *values)
{
	int		i;
	int		j;
	int		k;
	char	**split;

	i = 0;
	k = 0;
	while (argv[++i])
	{
		j = 0;
		split = ft_split(argv[i], ' ');
		if (!split)
			return (1);
		while (split[j])
		{
			if (parse_validate(split[j]))
			{
				ft_free(split);
				return (1);
			}
			values[k++] = ft_atol(split[j++]);
		}
		ft_free(split);
	}
	return (0);
}

static int	parse_duplicates(long *values, int total)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (i < total - 1)
	{
		j = i + 1;
		while (j < total)
		{
			if (values[i] == values[j])
				return (1);
			j++;
		}
		i++;
	}
	return (0);
}

long	*parsing(char **argv, int *total)
{
	long	*values;

	*total = parse_count(argv);
	if (*total <= 0)
		return (NULL);
	values = ft_calloc(*total, sizeof(long));
	if (!values)
		return (NULL);
	if (parse_fill(argv, values) || parse_duplicates(values, *total))
	{
		free(values);
		return (NULL);
	}
	return (values);
}
