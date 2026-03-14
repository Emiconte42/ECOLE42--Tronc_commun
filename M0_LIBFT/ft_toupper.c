/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 16:45:43 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 17:41:41 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_toupper(int c)
{
	if (c >= 'a' && c <= 'z')
		return (c - 32);
	return (c);
}
/*
#include <stdio.h>
int main(void)
{
    printf("A : %d\n", ft_toupper('A'));
    printf("b : %d\n", ft_toupper('b'));
    printf("6 : %d\n", ft_toupper('6'));
    printf("* : %d\n", ft_toupper('*'));
}
*/