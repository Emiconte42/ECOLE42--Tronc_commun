/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 15:29:43 by emiconte          #+#    #+#             */
/*   Updated: 2026/01/13 20:35:26 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	len_substr(char const *s, unsigned int start, size_t len)
{
	size_t	l;

	l = 0;
	while (s[l])
		l++;
	if ((size_t)start >= l)
		return (0);
	if (len > l - (size_t)start)
		return (l - (size_t)start);
	return (len);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	size_t	l;
	char	*sub;

	if (!s)
		return (NULL);
	l = len_substr(s, start, len);
	sub = malloc(l + 1);
	if (!sub)
		return (NULL);
	i = 0;
	while (i < l)
	{
		sub[i] = s[start + i];
		i++;
	}
	sub[i] = '\0';
	return (sub);
}
