/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 16:45:48 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 17:41:56 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		return (c + 32);
	return (c);
}
/*
#include <stdio.h>
int main(void)
{
    printf("A : %d\n", ft_tolower('A'));
    printf("b : %d\n", ft_tolower('b'));
    printf("6 : %d\n", ft_tolower('6'));
    printf("* : %d\n", ft_tolower('*'));
}
*/