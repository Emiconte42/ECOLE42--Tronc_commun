/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 19:46:51 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 18:15:01 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, char *src, size_t siz)
{
	size_t	i;
	size_t	len;

	i = 0;
	len = 0;
	while (src[len])
		len++;
	if (siz == 0)
		return (len);
	while (i < (siz - 1) && src[i])
	{
		dst[i] = src[i];
		i++;
	}
	if (siz > 0)
		dst[i] = '\0';
	return (len);
}
/*
#include <stdio.h>
int main(void)
{
	// afficher "hello"
    char dst[20];
    ft_strlcpy(dst, "hello", 20);
    printf("Test 1: %s\n", dest);

	// afficher "wo"
    ft_strlcpy(dest, "world", 3);
    printf("Test 2: %s\n", dest); 
	
	// rien, mais src est renvoyé
    ft_strlcpy(dest, "abcdedf", 0);
    printf("Test 3: %s\n", dest); 
    return (0);
}
*/