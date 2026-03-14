/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_triage.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 14:44:37 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/13 13:22:17 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pushswap.h"

int	find_position(t_node *stack, int index_search)
{
	int		position;
	t_node	*current;

	position = 0;
	current = stack;
	while (current)
	{
		if (current->index == index_search)
			return (position);
		position++;
		current = current->next;
	}
	return (-1);
}

int	is_in_top_half(t_node *stack, int index_search)
{
	int	position;
	int	size;

	position = find_position(stack, index_search);
	size = list_size(stack);
	if (position <= size / 2)
		return (1);
	else
		return (0);
}
