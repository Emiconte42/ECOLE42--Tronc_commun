/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 22:05:47 by emiconte          #+#    #+#             */
/*   Updated: 2026/01/13 20:37:24 by emiconte         ###   ########.fr       */
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

static char	*create_words(const char *s, int start, int end)
{
	char	*mot;
	int		i;

	mot = malloc((end - start + 1) * sizeof(char));
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

static void	ft_free(char **tab, int k)
{
	while (k >= 0)
	{
		free(tab[k]);
		k--;
	}
	free(tab);
}

static int	fill_array(char **tab, const char *s, char sep, int *k)
{
	int	i;
	int	start;

	i = 0;
	while (s[i])
	{
		while (s[i] && s[i] == sep)
			i++;
		start = i;
		while (s[i] && s[i] != sep)
			i++;
		if (i > start)
		{
			tab[*k] = create_words(s, start, i);
			if (!tab[*k])
				return (1);
			(*k)++;
		}
	}
	return (0);
}

char	**ft_split(char const *s, char sep)
{
	char	**tab;
	int		k;

	if (!s)
		return (NULL);
	tab = malloc((count_words(s, sep) + 1) * sizeof(char *));
	if (!tab)
		return (NULL);
	k = 0;
	if (fill_array(tab, s, sep, &k))
	{
		ft_free(tab, k - 1);
		return (NULL);
	}
	tab[k] = NULL;
	return (tab);
}
