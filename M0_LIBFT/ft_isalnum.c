/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalnum.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 14:49:22 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 18:27:59 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalnum(int c)
{
	if (ft_isalpha(c) == 1 || ft_isdigit(c) == 1)
		return (1);
	else
		return (0);
}

/*
#include <stdio.h>
int main(void)
{
    printf("A : %d\n", ft_isalnum('A'));
    printf("b : %d\n", ft_isalnum('b'));
    printf("6 : %d\n", ft_isalnum('6'));
    printf("* : %d\n", ft_isalnum('*'));
}
*/