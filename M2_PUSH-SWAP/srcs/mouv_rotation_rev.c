/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mouv_rotation_rev.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 16:13:27 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/13 16:16:31 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pushswap.h"

static void	rotation_reverse(t_node **stack)
{
	t_node	*second_to_last;
	t_node	*last;

	if (!stack || !*stack || !(*stack)->next)
		return ;
	second_to_last = NULL;
	last = *stack;
	while (last->next)
	{
		second_to_last = last;
		last = last->next;
	}
	second_to_last->next = NULL;
	last->next = *stack;
	*stack = last;
}

void	rra(t_node **stack)
{
	rotation_reverse(stack);
	ft_printf("rra\n");
}

void	rrb(t_node **stack)
{
	rotation_reverse(stack);
	ft_printf("rrb\n");
}

void	rrr(t_node **stack_a, t_node **stack_b)
{
	rotation_reverse(stack_a);
	rotation_reverse(stack_b);
	ft_printf("rrr\n");
}
