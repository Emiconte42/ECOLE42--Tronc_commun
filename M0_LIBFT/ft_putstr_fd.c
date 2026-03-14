/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 09:48:24 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 18:22:40 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putstr_fd(char *s, int fd)
{
	int	len;

	len = 0;
	while (s[len])
	{
		ft_putchar_fd(s[len], fd);
		len ++;
	}
}
/*
int	main(void)
{
	ft_putstr_fd("Hello", 1);
	ft_putchar_fd('\n', 1);
	ft_putstr_fd("Erreur !", 2);
	return (0);
}
*/