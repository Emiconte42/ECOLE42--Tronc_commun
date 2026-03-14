/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 14:53:29 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 18:24:33 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*ptr;

	i = 0;
	ptr = (unsigned char *)s;
	while (i < n)
	{
		ptr[i] = c;
		i++;
	}
	return (s);
}

/*
#include <stdio.h>
int main(void)
{
    char str[20] = "Hello World";

    printf("Avant : '%s'\n", str);
    printf("Après : '%s'\n", (char *)ft_memset(str + 6, '*', 3));
	//Resultat : Avant : 'Hello World' // Après : '***ld'
    return (0);
}
*/