/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   algo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 15:10:56 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/13 13:24:05 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pushswap.h"

void	algo(t_node **stack_a, t_node **stack_b, int size)
{
	if (size == 2)
		two(stack_a, size);
	else if (size == 3)
		three(stack_a, size);
	else if (size == 4 || size == 5)
		four_and_five(stack_a, stack_b, size);
	else if (size > 5)
		radix(stack_a, stack_b, size);
}
