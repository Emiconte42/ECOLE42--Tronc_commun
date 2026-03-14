/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 12:18:12 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 18:27:52 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalpha(int c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return (1);
	else
		return (0);
}
/*
#include <stdio.h>
int main(void)
{
    printf("A : %d\n", ft_isalpha('A'));
    printf("b : %d\n", ft_isalpha('b'));
    printf("6 : %d\n", ft_isalpha('6'));
    printf("* : %d\n", ft_isalpha('*'));
}
*/