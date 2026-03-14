/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 14:35:25 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 18:15:06 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	int			i;
	const char	*last;

	i = 0;
	last = NULL;
	while (s[i])
	{
		if (s[i] == (char)c)
			last = &s[i];
		i++;
	}
	if ((char)c == '\0')
		return ((char *)&s[i]);
	return ((char *)last);
}
/*
#include <stdio.h>
int	main(void)
{
	char ptr[] = "Yo la team est ce que ca va bien ?!";
	char c = 'c';
	char *trouve;
	trouve = ft_strrchr(ptr, c);
	if (trouve)
		printf("%s", trouve);
	else
		printf("n'a rien trouve");	
	return(0);
}
*/