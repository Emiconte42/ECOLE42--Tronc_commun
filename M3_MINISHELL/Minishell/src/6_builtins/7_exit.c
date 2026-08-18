/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   7_exit.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 15:40:06 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 10:06:41 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_numeric(char *str)
{
	int			i;
	long long	number;
	long long	prev;

	i = 0;
	prev = 0;
	number = 0;
	if (!str || !str[0])
		return (ERROR);
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (ERROR);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (ERROR);
		prev = number;
		number = number * 10 + (str[i] - '0');
		if (number / 10 != prev)
			return (ERROR);
		i++;
	}
	return (SUCCESS);
}

static void	exit_with_status(t_shell *shell, char *arg)
{
	long long	status;

	if (is_numeric(arg) != SUCCESS)
	{
		ft_putendl_fd("exit", STDERR_FILENO);
		ft_putstr_fd("exit: ", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putendl_fd(": numeric argument required", STDERR_FILENO);
		clean_gc_history(shell);
		close_and_dup_std(&shell, 0);
		exit(2);
	}
	status = ft_atoll(arg) % 256;
	if (status < 0)
		status += 256;
	clean_gc_history(shell);
	close_and_dup_std(&shell, 0);
	exit(status);
}

int	ft_exit(t_cmd *cmd, t_shell *shell)
{
	int	in_pipe;

	in_pipe = (shell->stdin == -1 && shell->stdout == -1);
	if (cmd->args[1] == NULL)
	{
		if (!in_pipe)
			ft_putendl_fd("exit", STDERR_FILENO);
		clean_gc_history(shell);
		close_and_dup_std(&shell, 0);
		exit(shell->exit_status);
	}
	if (is_numeric(cmd->args[1]) != SUCCESS)
		exit_with_status(shell, cmd->args[1]);
	if (cmd->args[2])
	{
		if (!in_pipe)
			ft_putendl_fd("exit", STDERR_FILENO);
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		shell->exit_status = 1;
		return (ERROR);
	}
	if (!in_pipe)
		ft_putendl_fd("exit", STDERR_FILENO);
	exit_with_status(shell, cmd->args[1]);
	return (SUCCESS);
}
