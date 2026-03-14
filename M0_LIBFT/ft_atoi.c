/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 11:37:12 by emiconte          #+#    #+#             */
/*   Updated: 2025/11/26 17:29:07 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_atoi(const char *nptr)
{
	int	result;
	int	sign;
	int	i;

	i = 0;
	sign = 1;
	result = 0;
	while ((nptr[i] == ' ') || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if (nptr[i] == '+' || nptr[i] == '-')
	{
		if (nptr[i] == '-')
			sign = -1;
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		result = result * 10 + (nptr[i] - '0');
		i++;
	}
	return (result * sign);
}
/*
int	main(void)
{
	char	nptr1[] = "123";
	char	nptr2[] = "0";
	char	nptr3[] = "+42";
	char	nptr4[] = "-42";
	char	nptr5[] = "   56";
	char	nptr6[] = "\t\n 78";
	char	nptr7[] = "123abc";
	char	nptr8[] = "-99xyz";
	char	nptr9[] = "abc123";
	char	nptr10[] = "";
	char	nptr11[] = "-";
	char	nptr12[] = "+";
	char	nptr13[] = "000123";
	char	nptr14[] = "-0007";
	char	nptr15[] = "2147483648";
	char	nptr16[] = "-2147483649";
	printf("'%s' -> %d\n", nptr1, ft_atoi(nptr1));
	printf("'%s' -> %d\n", nptr2, ft_atoi(nptr2));
	printf("'%s' -> %d\n", nptr3, ft_atoi(nptr3));
	printf("'%s' -> %d\n", nptr4, ft_atoi(nptr4));
	printf("'%s' -> %d\n", nptr5, ft_atoi(nptr5));
	printf("'%s' -> %d\n", nptr6, ft_atoi(nptr6));
	printf("'%s' -> %d\n", nptr7, ft_atoi(nptr7));
	printf("'%s' -> %d\n", nptr8, ft_atoi(nptr8));
	printf("'%s' -> %d\n", nptr9, ft_atoi(nptr9));
	printf("'%s' -> %d\n", nptr10, ft_atoi(nptr10));
	printf("'%s' -> %d\n", nptr11, ft_atoi(nptr11));
	printf("'%s' -> %d\n", nptr12, ft_atoi(nptr12));
	printf("'%s' -> %d\n", nptr13, ft_atoi(nptr13));
	printf("'%s' -> %d\n", nptr14, ft_atoi(nptr14));
	printf("'%s' -> %d\n", nptr15, ft_atoi(nptr15));
	printf("'%s' -> %d\n", nptr16, ft_atoi(nptr16));

	return (0);
}
*/
