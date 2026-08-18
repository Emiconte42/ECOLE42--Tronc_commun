/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 12:20:20 by emiconte          #+#    #+#             */
/*   Updated: 2026/01/13 13:21:51 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// Je declare un noeud de type node
// Je malloc, je verifie et je protege
// je verifie que mon contenu est = a content
// puis je fais en sorte que le prochain noeud (next) soit null

t_list	*ft_lstnew(void *content)
{
	t_list	*first_node;

	first_node = malloc(sizeof(t_list));
	if (!first_node)
		return (NULL);
	first_node -> content = content;
	first_node -> next = NULL;
	return (first_node);
}
