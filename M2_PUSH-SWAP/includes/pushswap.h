/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pushswap.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 13:28:07 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/13 15:37:17 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSHSWAP_H
# define PUSHSWAP_H

# include "../libs/libft/libft.h"
# include <unistd.h>
# include <stdlib.h>

typedef struct s_node
{		
	int				values;
	int				index;
	struct s_node	*next;
}					t_node;

long		*parsing(char **argv, int *total);
t_node		*init_stack(long *values, int total);
void		assign_index(t_node **stack);
int			is_sorted(t_node *stack);
void		algo(t_node **stack_a, t_node **stack_b, int size);

// utils_list.c
t_node		*list_create_node(int value);
void		list_add_front(t_node **stack, t_node *new_node);
t_node		*list_node_last(t_node *stack);
void		list_add_back(t_node **stack, t_node *new_node);
int			list_size(t_node *stack);

//utils_triage
int			find_position(t_node *stack, int index_search);
int			is_in_top_half(t_node *stack, int index_search);

//operation.c
void		sa(t_node **stack);
void		sb(t_node **stack);
void		ss(t_node **stack_a, t_node **stack_b);
void		ra(t_node **stack);
void		rb(t_node **stack);
void		rr(t_node **stack_a, t_node **stack_b);
void		rra(t_node **stack);
void		rrb(t_node **stack);
void		rrr(t_node **stack_a, t_node **stack_b);
void		pa(t_node **stack_a, t_node **stack_b);
void		pb(t_node **stack_a, t_node **stack_b);

//operation.c
void		two(t_node **stack, int size);
void		three(t_node **stack, int size);
void		four_and_five(t_node **stack_a, t_node **stack_b, int size);
void		radix(t_node **stack_a, t_node **stack_b, int size);

//free.c
void		ft_free(char **str);
void		list_clear(t_node **stack);

#endif