/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 09:30:32 by emiconte          #+#    #+#             */
/*   Updated: 2026/03/03 19:21:48 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
# include <stdio.h>
# include <stdlib.h>

void	ft_putchar_fd(char c, int fd)
{
	write (fd, &c, 1);
}
// // le 1 ecrit 'A' sur la sortie standard
// // le 2 ecrit 'E' sur la sortie d'erreur
int	main(void)
{
	ft_putchar_fd('A', 1);
    ft_putchar_fd('\n', 1);
	ft_putchar_fd('E', 2);

	printf("\nSalut les gens");
}
