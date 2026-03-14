/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 15:44:18 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 17:59:33 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	if (i == n)
		return (0);
	return ((unsigned char) s1[i] - (unsigned char)s2[i]);
}

/*
#include <stdio.h>
int main(void)
{
    printf("%d\n", ft_strncmp("hello", "hello", 5));
    printf("%d\n", ft_strncmp("hello", "helxo", 4));
    printf("%d\n", ft_strncmp("abc", "abd", 3));
    printf("%d\n", ft_strncmp("abc", "ab", 3));
    printf("%d\n", ft_strncmp("abc", "abc", 0));
    return (0);
}
*/