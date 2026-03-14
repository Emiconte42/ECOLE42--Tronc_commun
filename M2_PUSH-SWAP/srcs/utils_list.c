/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 11:39:07 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/13 13:21:01 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pushswap.h"
#include "../libs/libft/libft.h"

t_node	*list_create_node(int value)
{
	t_node	*new_node;

	new_node = malloc(sizeof(t_node));
	if (!new_node)
		return (NULL);
	new_node->values = value;
	new_node->next = NULL;
	return (new_node);
}

void	list_add_front(t_node **stack, t_node *new_node)
{
	if (!new_node)
		return ;
	new_node->next = *stack;
	*stack = new_node;
}

t_node	*list_node_last(t_node *stack)
{
	t_node	*current;

	if (!stack)
		return (NULL);
	current = stack;
	while (current->next != NULL)
	{
		current = current->next;
	}
	return (current);
}

void	list_add_back(t_node **stack, t_node *new_node)
{
	t_node	*last;

	if (!new_node)
		return ;
	if (!*stack)
	{
		*stack = new_node;
		return ;
	}
	last = list_node_last(*stack);
	last->next = new_node;
}

int	list_size(t_node *stack)
{
	int	counter;

	counter = 0;
	while (stack)
	{
		counter++;
		stack = stack->next;
	}
	return (counter);
}

// void	print_list(t_node *stack)
// {
// 	while (stack)
// 	{
// 		ft_printf("%d -> ", stack->values);
// 		stack = stack->next;
// 	}
// 	ft_printf("NULL\n");
// }