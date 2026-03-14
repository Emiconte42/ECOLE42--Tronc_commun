/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_outils2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 13:17:49 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/09 14:03:48 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putnbr_hexa_lowercase(unsigned long nb)
{
	int		count;
	char	*base;

	base = "0123456789abcdef";
	count = 0;
	if (nb >= 16)
		count = count + ft_putnbr_hexa_lowercase(nb / 16);
	count = count + ft_putchar(base[nb % 16]);
	return (count);
}

int	ft_putnbr_hexa_uppercase(unsigned long nb)
{
	int		count;
	char	*base;

	base = "0123456789ABCDEF";
	count = 0;
	if (nb >= 16)
		count = count + ft_putnbr_hexa_uppercase(nb / 16);
	count = count + ft_putchar(base[nb % 16]);
	return (count);
}
