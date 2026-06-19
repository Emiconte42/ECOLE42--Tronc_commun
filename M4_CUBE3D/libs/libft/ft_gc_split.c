/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gc_split.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 09:39:21 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/09 11:18:37 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_words(const char *s, char sep)
{
	int	i;
	int	mots;

	i = 0;
	mots = 0;
	while (s[i])
	{
		while (s[i] && s[i] == sep)
			i++;
		if (s[i] && s[i] != sep)
			mots++;
		while (s[i] && s[i] != sep)
			i++;
	}
	return (mots);
}

static char	*create_words(const char *s, int start, int end, t_list **gc)
{
	char	*mot;
	int		i;

	mot = ft_gc_calloc(end - start + 1, sizeof(char), gc);
	if (!mot)
		return (NULL);
	i = 0;
	while (start < end)
	{
		mot[i] = s[start];
		i++;
		start++;
	}
	mot[i] = '\0';
	return (mot);
}

static int	fill_array(char **tab, const char *s, char sep, t_list **gc)
{
	int	i;
	int	start;
	int	k;

	i = 0;
	k = 0;
	while (s[i])
	{
		while (s[i] && s[i] == sep)
			i++;
		start = i;
		while (s[i] && s[i] != sep)
			i++;
		if (i > start)
		{
			tab[k] = create_words(s, start, i, gc);
			if (!tab[k])
				return (1);
			k++;
		}
	}
	return (0);
}

char	**ft_gc_split(char const *s, char sep, t_list **gc)
{
	char	**tab;

	if (!s)
		return (NULL);
	tab = ft_gc_calloc(count_words(s, sep) + 1, sizeof(char *), gc);
	if (!tab)
		return (NULL);
	if (fill_array(tab, s, sep, gc))
		return (NULL);
	return (tab);
}
