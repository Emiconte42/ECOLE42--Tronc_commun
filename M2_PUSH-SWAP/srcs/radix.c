/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   radix.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:18:32 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/13 15:39:41 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pushswap.h"

static int	get_max_bits(int size)
{
	int	index_max;
	int	max_bits;

	index_max = size - 1;
	max_bits = 0;
	while (index_max > 0)
	{
		index_max = index_max >> 1;
		max_bits++;
	}
	return (max_bits);
}

void	radix(t_node **stack_a, t_node **stack_b, int size)
{
	int	max_bits;
	int	bit;
	int	count;

	max_bits = get_max_bits(size);
	bit = 0;
	while (bit < max_bits)
	{
		count = 0;
		while (count < size)
		{
			if (((*stack_a)->index >> bit) & 1)
				ra(stack_a);
			else
				pb(stack_a, stack_b);
			count++;
		}
		while (*stack_b)
			pa(stack_a, stack_b);
		bit++;
	}
}
