/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialisation.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 09:19:46 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/13 15:39:16 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pushswap.h"

int	is_sorted(t_node *stack)
{
	while (stack && stack->next)
	{
		if (stack->values > stack->next->values)
			return (0);
		stack = stack->next;
	}
	return (1);
}

void	assign_index(t_node **stack)
{
	t_node	*current;
	t_node	*compare;
	int		index;

	current = *stack;
	while (current)
	{
		index = 0;
		compare = *stack;
		while (compare)
		{
			if (compare->values < current->values)
				index++;
			compare = compare->next;
		}
		current->index = index;
		current = current->next;
	}
}

t_node	*init_stack(long *values, int total)
{
	t_node	*stack_a;
	t_node	*new;
	int		i;

	i = 0;
	stack_a = NULL;
	while (i < total)
	{
		new = list_create_node(values[i]);
		if (!new)
		{
			list_clear(&stack_a);
			return (NULL);
		}
		list_add_back(&stack_a, new);
		i++;
	}
	return (stack_a);
}
