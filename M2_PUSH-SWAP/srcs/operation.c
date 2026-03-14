/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operation.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 14:17:17 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/13 16:10:15 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pushswap.h"

void	two(t_node **stack, int size)
{
	t_node	*current;

	if (!stack || !*stack || size < 2)
		return ;
	current = *stack;
	if (current->values < current->next->values)
		return ;
	if (current->values > current->next->values)
		sa(stack);
}

void	three(t_node **stack, int size)
{
	int		one;
	int		two;
	int		three;
	t_node	*current;

	if (!stack || !*stack || size < 3)
		return ;
	current = *stack;
	one = current->index;
	two = current->next->index;
	three = current->next->next->index;
	if (one > two && two < three && one < three)
		sa(stack);
	if (one < two && two > three && one > three)
		rra(stack);
	if (one > two && two < three && one > three)
		ra(stack);
	if (one < two && two > three && one < three)
		sa(stack);
	if (one < two && two > three && one < three)
		ra(stack);
	if (one > two && two > three && one > three)
		sa(stack);
	if (one > two && two > three && one > three)
		rra(stack);
}

void	four_and_five(t_node **stack_a, t_node **stack_b, int size)
{
	while ((*stack_a)->index != 0)
	{
		if (is_in_top_half(*stack_a, 0))
			ra(stack_a);
		else
			rra(stack_a);
	}
	pb(stack_a, stack_b);
	if (size == 5)
	{
		while ((*stack_a)->index != 1)
		{
			if (is_in_top_half(*stack_a, 1))
				ra(stack_a);
			else
				rra(stack_a);
		}
		pb(stack_a, stack_b);
	}
	three(stack_a, 3);
	pa(stack_a, stack_b);
	if (size == 5)
		pa(stack_a, stack_b);
}
