/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 13:50:35 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/13 15:38:46 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pushswap.h"

int	main(int argc, char **argv)
{
	long	*values;
	int		total;
	t_node	*stack_a;
	t_node	*stack_b;
	int		size;

	stack_b = NULL;
	if (argc <= 1)
		return (0);
	values = parsing(argv, &total);
	if (!values)
	{
		ft_putendl_fd("Error", 2);
		return (1);
	}
	stack_a = init_stack(values, total);
	size = list_size(stack_a);
	assign_index(&stack_a);
	if (!is_sorted(stack_a))
		algo(&stack_a, &stack_b, size);
	free(values);
	values = NULL;
	list_clear(&stack_a);
	list_clear(&stack_b);
	return (0);
}
