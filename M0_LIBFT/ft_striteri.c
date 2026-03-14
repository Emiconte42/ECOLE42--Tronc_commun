/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 16:02:45 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 18:20:02 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_striteri(char *s, void (*f)(unsigned int, char*))
{
	unsigned int	i;

	i = 0;
	if (!s || !f)
		return ;
	while (s[i])
	{
		f(i, &s[i]);
		i++;
	}
}
/*
void	test(unsigned int i, char *c)
{
	(void)i;        // on ignore l’index
	*c = *c - 32;   // met en majuscule
}

int main(void)
{
	char str[] = "bonjour";
	ft_striteri(str, test);
	printf("%s\n", str);
	return 0;
}
*/