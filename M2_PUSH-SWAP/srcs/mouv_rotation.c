/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mouv_rotation.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 16:11:11 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/13 16:16:58 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pushswap.h"

static void	rotation(t_node **stack)
{
	t_node	*first;
	t_node	*last;

	if (!stack || !*stack || !(*stack)->next)
		return ;
	first = *stack;
	last = list_node_last(*stack);
	*stack = first->next;
	last->next = first;
	first->next = NULL;
}

void	ra(t_node **stack)
{
	rotation(stack);
	ft_printf("ra\n");
}

void	rb(t_node **stack)
{
	rotation(stack);
	ft_printf("rb\n");
}

void	rr(t_node **stack_a, t_node **stack_b)
{
	rotation(stack_a);
	rotation(stack_b);
	ft_printf("rr\n");
}
