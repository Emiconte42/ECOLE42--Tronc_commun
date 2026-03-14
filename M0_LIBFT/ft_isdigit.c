/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 14:22:08 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 18:29:24 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	else
		return (0);
}
/*
#include <stdio.h>
int main(void)
{
    printf("A : %d\n", ft_isdigit('A'));
    printf("b : %d\n", ft_isdigit('b'));
    printf("6 : %d\n", ft_isdigit('6'));
    printf("* : %d\n", ft_isdigit('*'));
}
*/