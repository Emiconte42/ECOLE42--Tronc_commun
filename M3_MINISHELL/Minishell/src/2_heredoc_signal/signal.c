/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/15 16:20:20 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 11:59:09 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

volatile sig_atomic_t	g_signal = 0;

static void	handle_sigint_interactive(int sig)
{
	g_signal = sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

static void	handle_sigint_exec(int sig)
{
	g_signal = sig;
	write(1, "\n", 1);
}

static void	handle_sigquit_exec(int sig)
{
	g_signal = sig;
	write(1, "Quit (core dumped)\n", 19);
}

void	ft_handle_heredoc_sigint(int sig)
{
	g_signal = sig;
	close(STDIN_FILENO);
	write(2, "\n", 1);
}

void	set_signals(int context)
{
	if (context == SIG_INTERACTIVE)
	{
		signal(SIGINT, handle_sigint_interactive);
		signal(SIGQUIT, SIG_IGN);
	}
	else if (context == SIG_EXEC)
	{
		signal(SIGINT, handle_sigint_exec);
		signal(SIGQUIT, handle_sigquit_exec);
	}
	else if (context == MS_SIG_DEFAULT)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
	}
}
