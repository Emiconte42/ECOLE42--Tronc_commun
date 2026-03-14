/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 15:20:47 by emiconte          #+#    #+#             */
/*   Updated: 2025/12/10 15:21:01 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

// librairie
# include <stdarg.h>
# include <unistd.h>
# include <limits.h>

//Fonctions principales
int	ft_printf(const char *format, ...);
int	ft_format(char c, va_list args);

//Fonctions secondaires
int	ft_putchar(char c);
int	ft_putstr(char *s);
int	ft_putnbr(int nb);
int	ft_putnbr_unsigned(unsigned int nb);
int	ft_putnbr_hexa_lowercase(unsigned long int nb);
int	ft_putnbr_hexa_uppercase(unsigned long int nb);
int	ft_putptr(unsigned long ptr);

#endif