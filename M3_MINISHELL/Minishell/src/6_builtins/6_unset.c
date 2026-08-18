/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   6_unset.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 17:19:13 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/13 21:33:07 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	remove_env_var(t_shell *shell, int i)
{
	while (shell->env[i + 1])
	{
		shell->env[i] = shell->env[i + 1];
		i++;
	}
	shell->env[i] = NULL;
}

static void	unset_one(t_shell *shell, char *arg)
{
	int	i;
	int	name_len;

	i = 0;
	name_len = ft_strlen(arg);
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], arg, name_len) == 0
			&& (shell->env[i][name_len] == '='
			|| shell->env[i][name_len] == '\0'))
		{
			remove_env_var(shell, i);
			break ;
		}
		i++;
	}
}

int	ft_unset(t_cmd *cmd, t_shell *shell)
{
	int	i;
	int	ret;

	i = 1;
	ret = SUCCESS;
	if (cmd->args[1] == NULL)
		return (ret);
	while (cmd->args[i])
	{
		if (cmd->args[i][0] == '-')
		{
			ft_putstr_fd("unset: -", STDERR_FILENO);
			ft_putchar_fd(cmd->args[i][1], STDERR_FILENO);
			ft_putstr_fd(": invalid option\n", STDERR_FILENO);
			ft_putstr_fd("unset: usage: unset [-f] [-v] [-n] [name ...]\n",
				STDERR_FILENO);
			shell->exit_status = 2;
			return (2);
		}
		if (is_valid_name(cmd->args[i]) == SUCCESS)
			unset_one(shell, cmd->args[i]);
		i++;
	}
	return (ret);
}
