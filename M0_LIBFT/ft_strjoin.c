/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 22:02:53 by emiconte          #+#    #+#             */
/*   Updated: 2025/12/05 11:12:27 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*totalstr;
	size_t	i;
	size_t	j;

	if (!s1 && !s2)
		return (NULL);
	totalstr = malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
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
#include <stdio.h>
int main(void)
{
	char *res = ft_strjoin("Hello ", "42");
	printf("Resultat : %s\n", res); // Hello 42
	free(res);
	return 0;
}
*/