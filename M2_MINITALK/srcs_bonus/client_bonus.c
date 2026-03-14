/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 13:08:34 by emiconte          #+#    #+#             */
/*   Updated: 2026/02/20 14:41:40 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minitalk.h"

static int	g_received = 0;

void	handler(int signum)
{
	(void)signum;
	g_received = 1;
}

static void	send_char(int pid, unsigned char c)
{
	int	bit;

	bit = 7;
	while (bit >= 0)
	{
		if (((c >> bit) & 1) == 1)
			kill(pid, SIGUSR1);
		else
			kill(pid, SIGUSR2);
		usleep(1000);
		bit--;
	}
}

int	main(int argc, char **argv)
{
	int	pid;
	int	i;

	signal(SIGUSR1, handler);
	if (argc != 3)
	{
		ft_putendl_fd("Error: ./client [PID] [message]", 2);
		return (1);
	}
	pid = ft_atoi(argv[1]);
	if (pid <= 0 || kill(pid, 0) == -1)
	{
		ft_putendl_fd("Error: invalid PID", 2);
		return (1);
	}
	i = 0;
	while (argv[2][i])
		send_char(pid, argv[2][i++]);
	send_char(pid, '\0');
	while (!g_received)
		pause();
	write(1, "OK received\n", 12);
	return (0);
}
