/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 14:59:45 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 18:29:19 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isascii(int c)
{
	if (c >= 0 && c <= 127)
		return (1);
	else
		return (0);
}
/*
#include <stdio.h>
int main(void)
{
    printf("A : %d\n", ft_isascii('A'));
    printf("b : %d\n", ft_isascii('b'));
    printf("6 : %d\n", ft_isascii('6'));
    printf("* : %d\n", ft_isascii('*'));
}
*/