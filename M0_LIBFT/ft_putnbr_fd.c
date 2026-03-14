/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 10:29:55 by emiconte          #+#    #+#             */
/*   Updated: 2025/12/16 12:06:23 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	long	nb;

	nb = n;
	if (nb < 0)
	{
		ft_putchar_fd('-', fd);
		nb = -nb;
	}
	if (nb < 10)
		ft_putchar_fd(nb + '0', fd);
	else
	{
		ft_putnbr_fd(nb / 10, fd);
		ft_putchar_fd(nb % 10 + '0', fd);
	}
}
// 
// int	main(void)
// {
	// ft_putnbr_fd(42, 1);
	// ft_putchar_fd('\n', 1);
// 
	// ft_putnbr_fd(-123, 1);
	// ft_putchar_fd('\n', 1);
// 
	// ft_putnbr_fd(0, 1);
	// ft_putchar_fd('\n', 1);
// 
	// return (0);
// }
// 