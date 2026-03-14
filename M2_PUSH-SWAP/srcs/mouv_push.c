/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mouv_push.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 16:14:29 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/13 16:17:32 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pushswap.h"

void	pa(t_node **stack_a, t_node **stack_b)
{
	t_node	*tmp;
	t_node	*current;

	if (!stack_a || !*stack_b)
		return ;
	current = *stack_b;
	tmp = *stack_b;
	*stack_b = current->next;
	list_add_front(stack_a, tmp);
	write(1, "pa\n", 3);
}

void	pb(t_node **stack_a, t_node **stack_b)
{
	t_node	*tmp;
	t_node	*current;

	if (!stack_b || !*stack_a)
		return ;
	current = *stack_a;
	tmp = *stack_a;
	*stack_a = current->next;
	list_add_front(stack_b, tmp);
	write(1, "pb\n", 3);
}
