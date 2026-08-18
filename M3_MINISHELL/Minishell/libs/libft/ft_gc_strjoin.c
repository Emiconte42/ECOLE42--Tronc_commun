/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gc_strjoin.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 22:02:53 by emiconte          #+#    #+#             */
/*   Updated: 2026/05/12 16:22:29 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_gc_strjoin(const char *s1, const char *s2, t_list **gc)
{
	char	*join;

	join = ft_strjoin(s1, s2);
	if (!join)
		return (NULL);
	if (ft_gc_add(gc, join))
	{
		free(join);
		return (NULL);
	}
	return (join);
}
