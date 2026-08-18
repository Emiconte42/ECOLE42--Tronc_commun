/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gc_itoa.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 12:55:53 by emiconte          #+#    #+#             */
/*   Updated: 2026/05/13 18:33:37 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	neg(char *str, long *nb)
{
	str[0] = '-';
	*nb = -*nb;
}

char	*ft_gc_itoa(int n, t_list **gc)
{
	long	nb;
	int		len;
	char	*str;

	nb = (long)n;
	len = (nb <= 0);
	while (n)
	{
		n /= 10;
		len++;
	}
	str = ft_gc_calloc(1, len + 1, gc);
	if (!str)
		return (NULL);
	str[len] = '\0';
	if (nb < 0)
		neg(str, &nb);
	if (nb == 0)
		str[0] = '0';
	while (nb)
	{
		str[--len] = (nb % 10) + '0';
		nb /= 10;
	}
	return (str);
}
