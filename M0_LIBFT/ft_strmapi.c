/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 12:28:15 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/19 15:56:04 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char			*new;
	unsigned int	i;
	unsigned int	len;

	i = 0;
	len = 0;
	while (s[len])
		len ++;
	new = malloc((len + 1) * sizeof(char));
	if (!new)
		return (NULL);
	while (i < len)
	{
		new[i] = f(i, s[i]);
		i++;
	}
	new[i] = '\0';
	return (new);
}
/*
char	test(unsigned int i, char c)
{
	(void)i;
	return (c - 32); // met en majuscule
}

int main(void)
{
	char *res = ft_strmapi("bonjour", test);

	if (!res)
		return 1;

	printf("%s\n", res);
	free(res);
	return 0;
}
*/