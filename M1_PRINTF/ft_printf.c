/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 12:13:35 by emiconte          #+#    #+#             */
/*   Updated: 2025/12/10 16:14:11 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_format(char c, va_list args)
{
	if (c == 'c')
		return (ft_putchar(va_arg(args, int)));
	else if (c == 's')
		return (ft_putstr(va_arg(args, char *)));
	else if (c == 'p')
		return (ft_putptr(va_arg(args, unsigned long)));
	else if (c == 'd' || c == 'i')
		return (ft_putnbr(va_arg(args, int)));
	else if (c == 'u')
		return (ft_putnbr_unsigned(va_arg(args, unsigned int)));
	else if (c == 'x')
		return (ft_putnbr_hexa_lowercase(va_arg(args, unsigned int)));
	else if (c == 'X')
		return (ft_putnbr_hexa_uppercase(va_arg(args, unsigned int)));
	else if (c == '%')
		return (ft_putchar('%'));
	return (0);
}

int	ft_printf(const char *format, ...)
{
	va_list	args;
	int		i;
	int		count;

	va_start (args, format);
	i = 0;
	count = 0;
	if (!format)
		return (-1);
	while (format[i])
	{
		if (format[i] == '%')
		{
			count = count + ft_format(format[i + 1], args);
			i++;
		}
		else
			count = count + ft_putchar(format[i]);
		i++;
	}
	va_end(args);
	return (count);
}
// #include<stdio.h>
// int main(void)
// {
//		printf("\nCaractere print\n");
//		ft_printf("%p \n", (void *)"");
//		printf("%p", (void *)"");
//
//		printf("\nString print\n");
//		ft_printf("+++ MY PRINTF ++++ : %s loulou\n", "world");
//		printf("+++ OG  PRINTF ++++ : %s loulou\n", "world");
//
//		printf("\nNumber print\n");
//		int m = ft_printf("+++ MY PRINTF ++++ : %d \n", -41232);
//		int n = printf("+++ OG PRINTF ++++ : %d \n", -41232);
//		printf("%d \n", m);
//		printf("%d \n", n);
//
//		printf("\nNumber unsigned int print\n");
//		int o = ft_printf("+++ MY PRINTF ++++ : %u \n", 394957295);
//		int p = printf("+++ OG PRINTF ++++ : %u \n", 394957295);
//		printf("%d \n", o);
//		printf("%d \n", p);
//
//		printf("\nlowercase\n");
//		int q = ft_printf("+++ MY PRINTF ++++ : %x \n", 213456);
//		int r = printf("+++ OG PRINTF ++++ : %x \n", 213456);
//		printf("%x \n", q);
//		printf("%x \n", r);
//
//		printf("\nuppercase\n");
//		int s = ft_printf("+++ MY PRINTF ++++ : %X \n", 213456);
//		int t = printf("+++ OG PRINTF ++++ : %X \n", 213456);
//		printf("%X \n", s);
//		printf("%X \n", t);
//
//		printf("putptr\n");
//		int u = ft_printf("de %p \n", "");
//		int v = printf("de %p \n", "");
//		printf("%X \n", u);
//		printf("%X \n", v);
//
//		printf("\nrien\n");
//		int u = ft_printf("+++ MY PRINTF ++++ : %% \n");
//		int v = printf("+++ OG PRINTF ++++ : %u \n");
//		printf("%d \n", u);
//		printf("%d \n", v);
// }
