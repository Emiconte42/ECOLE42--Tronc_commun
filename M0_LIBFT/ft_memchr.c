/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 15:37:47 by emiconte          #+#    #+#             */
/*   Updated: 2025/12/02 13:27:06 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char		*ptr;
	size_t					i;

	ptr = (const unsigned char *)s;
	i = 0;
	while (i < n)
	{
		if (ptr[i] == (unsigned char)c)
			return ((void *)(ptr + i));
		i++;
	}
	return (NULL);
}
/*
int main(void)
{
    char str[] = "abcde";
    char *result;

    result = (char *)ft_memchr(str, 'c', 5);
    if (result)
        printf("Trouvé à l'index %ld\n", result - str);
    else
        printf("Non trouvé\n");
    return (0);
}
*/
// Quand tu cherches 'o', tu trouves la premiere a l'index 1 (B = 0, o = 1 ...).