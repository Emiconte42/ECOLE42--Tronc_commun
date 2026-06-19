/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gc_calloc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 11:07:19 by emiconte          #+#    #+#             */
/*   Updated: 2026/05/14 16:40:45 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_gc_calloc(size_t count, size_t size, t_list	**gc)
{
	void	*ptr;

	if (count == 0 || size == 0)
		return (NULL);
	ptr = malloc(size * count);
	if (!ptr)
		return (NULL);
	ft_bzero(ptr, size * count);
	if (ft_gc_add(gc, ptr) != 0)
	{
		free(ptr);
		return (NULL);
	}
	return (ptr);
}
