/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:53:59 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/27 13:38:38 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t siz)
{
	size_t	dest_len;
	size_t	src_len;
	size_t	i;

	src_len = ft_strlen(src);
	dest_len = 0;
	while (dest_len < siz && dst[dest_len])
		dest_len++;
	if (dest_len == siz)
		return (siz + src_len);
	i = 0;
	while (dest_len + i < siz - 1 && src[i])
	{
		dst[dest_len + i] = src[i];
		i++;
	}
	dst[dest_len + i] = '\0';
	return (dest_len + src_len);
}

/*
#include <stdio.h>
int main(void)
{
    char dest[20] = "Hello ";
    char src[] = "World";
	printf("Avant : %s\n", dst);
	size_t ret;
	ret = ft_strlcat(dest, src, 20);
	printf("Après : %s\n", dst);
    return (0);
}
*/