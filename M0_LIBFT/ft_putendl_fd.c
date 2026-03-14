/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putendl_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 10:26:17 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/27 13:38:21 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putendl_fd(char *s, int fd)
{
	int	len;

	len = 0;
	while (s[len])
	{
		ft_putchar_fd(s[len], fd);
		len ++;
	}
	ft_putchar_fd('\n', fd);
}
/*
int	main(void)
{
	ft_putendl_fd("Bonjour", 1);
	ft_putendl_fd("Erreur détectée", 2);
	return (0);
}
*/