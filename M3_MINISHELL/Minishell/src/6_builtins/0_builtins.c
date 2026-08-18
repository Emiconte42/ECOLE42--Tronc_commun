/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   0_builtins.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 17:04:16 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 09:33:37 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	builtin(t_cmd *cmd, t_shell *shell)
{
	if (!cmd->args || !cmd->args[0])
		return (SUCCESS);
	if (ft_strcmp(cmd->args[0], "echo") == 0)
		shell->exit_status = ft_echo(cmd);
	else if (ft_strcmp(cmd->args[0], "cd") == 0)
		shell->exit_status = ft_cd(cmd, shell);
	else if (ft_strcmp(cmd->args[0], "pwd") == 0)
		shell->exit_status = ft_pwd(cmd, shell);
	else if (ft_strcmp(cmd->args[0], "env") == 0)
		shell->exit_status = ft_env(shell);
	else if (ft_strcmp(cmd->args[0], "export") == 0)
		shell->exit_status = ft_export(cmd, shell);
	else if (ft_strcmp(cmd->args[0], "unset") == 0)
		shell->exit_status = ft_unset(cmd, shell);
	else if (ft_strcmp(cmd->args[0], "exit") == 0)
		shell->exit_status = ft_exit(cmd, shell);
	return (SUCCESS);
}
