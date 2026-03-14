/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 21:49:41 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 18:29:05 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void			*ptr;
	size_t			i;

	i = 0;
	ptr = malloc(size * nmemb);
	if (!ptr)
		return (NULL);
	ft_bzero(ptr, size * nmemb);
	return (ptr);
}

/*
int	main(void)
{
	int	*tab;
	int	i;

	tab = ft_calloc(5, sizeof(int));
	if (!tab)
		return (1);
	printf("Résultat : ");
	i = 0;
	while (i < 5)
	{
		printf("%d", tab[i]);
		i++;
	}
	free(tab);
	return (0);
}
*/