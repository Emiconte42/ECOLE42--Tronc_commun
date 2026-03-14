/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 16:57:08 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 17:59:15 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	j;

	i = 0;
	if (little[i] == '\0')
		return ((char *) big);
	while (i < len && big[i])
	{
		if (big[i] == little[0])
		{
			j = 0;
			while (big[i + j] == little[j] && i + j < len && little[j])
				j++;
			if (little[j] == '\0')
				return ((char *)(big + i));
		}
		i++;
	}
	return (NULL);
}

/*
#include <stdio.h>
int main() 
{
	// Cas 1 : "little" est trouvé dans "big"
	char big1[] = "Bonjour tout le monde";
	char little1[] = "tout";
	char little2[] = "salut";
	char little3[] = "";
	char little4[] = "Bonjour tout";
	char *result1 = ft_strnstr(big1, little1, 20);
	char *result2 = ft_strnstr(big1, little2, 20);
	char *result3 = ft_strnstr(big1, little3, 20);
	char *result4 = ft_strnstr(big1, little4, 7);
	printf("1. str : %s, search : %s, Result : %s\n", big1, little1, result1);
	printf("2. str : %s, search : %s, Result : %s\n", big1, little2, result2);
	printf("3. str : %s, search : %s, Result : %s\n", big1, little3, result3);
	printf("4. str : %s, search : %s, Result : %s\n", big1, little4, result4);
	return (0);
}
*/
