/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gc_substr.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 15:29:43 by emiconte          #+#    #+#             */
/*   Updated: 2026/05/12 16:23:05 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_gc_substr(char const *s, unsigned int start, size_t len,
			t_list **gc)
{
	char	*sub;

	sub = ft_substr(s, start, len);
	if (!sub)
		return (NULL);
	if (ft_gc_add(gc, sub))
	{
		free(sub);
		return (NULL);
	}
	return (sub);
}
