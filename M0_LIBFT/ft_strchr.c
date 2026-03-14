/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 11:32:09 by emiconte          #+#    #+#             */
/*   Updated: 2025/12/16 12:06:33 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (s[i] == (char)c)
			return ((char *)&s[i]);
		i++;
	}
	if ((char)c == '\0')
		return ((char *)&s[i]);
	return (NULL);
}

// #include <stdio.h>
// int	main(void)
// {
// 	char ptr[] = "Yo la team est ce que ca va bien ?!";
// 	char c = 'e';
// 	char *trouve;

// 	trouve = ft_strchr(ptr, c);
// 	if (trouve)
// 		printf("%s", trouve);
// 	else
// 		printf("n'a rien trouve");	
// 	return(0);
// }