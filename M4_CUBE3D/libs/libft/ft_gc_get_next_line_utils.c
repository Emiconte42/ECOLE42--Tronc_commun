/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gc_get_next_line_utils.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 11:28:30 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/26 12:07:36 by hbelleuv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_gc_strjoin(char const *s1, char const *s2, t_list **gc)
{
	char	*totalstr;
	size_t	i;
	size_t	j;

	if (!s1 && !s2)
		return (NULL);
	totalstr = ft_gc_calloc(1, ft_strlen(s1) + ft_strlen(s2) + 1, gc);
	if (!totalstr)
		return (NULL);
	i = 0;
	while (s1 && s1[i] != '\0')
	{
		totalstr[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2 && s2[j] != '\0')
	{
		totalstr[i] = s2[j];
		j++;
		i++;
	}
	totalstr[i] = '\0';
	return (totalstr);
}

/*
char	*ft_gc_strdup(const char *s, t_list **gc)
{
	char		*dup;
	size_t		i;
	size_t		len;

	i = 0;
	len = ft_strlen(s);
	dup = ft_gc_calloc(1, (len + 1) * sizeof(char), gc);
	if (!dup)
		return (NULL);
	while (i < len)
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}
*/
