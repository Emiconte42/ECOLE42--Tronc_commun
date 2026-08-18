/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gc_strdup.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 21:58:06 by emiconte          #+#    #+#             */
/*   Updated: 2026/05/12 10:37:49 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_gc_strdup(const char *str, t_list **gc)
{
	char		*dup;

	dup = ft_strdup(str);
	if (!dup)
		return (NULL);
	if (ft_gc_add(gc, dup))
	{
		free(dup);
		return (NULL);
	}
	return (dup);
}
