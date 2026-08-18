/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 12:00:00 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 12:17:25 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	child_setup(t_shell **shell, int fd[2], t_cmd *cmd)
{
	close_unused_heredoc_redirs(*shell, cmd);
	close((*shell)->stdin);
	(*shell)->stdin = -1;
	close((*shell)->stdout);
	(*shell)->stdout = -1;
	if (cmd->next)
	{
		if (dup2(fd[1], STDOUT_FILENO) < 0 || close(fd[0]) < 0
			|| close(fd[1]) < 0)
			exit_child_perror(*shell, "dup stdout ", 1);
	}
}

static void	child_exec(t_cmd *cmd, t_shell *shell)
{
	set_signals(MS_SIG_DEFAULT);
	execve(cmd->path, cmd->args, cmd->env);
	exit_child_perror(shell, "execve ", 1);
}

static void	child_no_path(t_cmd *cmd, t_shell *shell)
{
	if (cmd->perm_denied && cmd->is_dir)
	{
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putendl_fd(": Is a directory", STDERR_FILENO);
		exit_child(shell, 126);
	}
	else if (cmd->perm_denied && !cmd->is_dir)
	{
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putendl_fd(": Permission denied", STDERR_FILENO);
		exit_child(shell, 126);
	}
	else if (cmd->not_found)
	{
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putendl_fd(": No such file or directory", STDERR_FILENO);
		exit_child(shell, 127);
	}
	else
	{
		if (cmd->args[0])
			ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putendl_fd(": command not found", STDERR_FILENO);
		exit_child(shell, 127);
	}
}

void	child(t_cmd *cmd, int fd[2], t_shell **shell)
{
	child_setup(shell, fd, cmd);
	if (cmd->redir && handle_redir(cmd, shell) == ERROR)
		exit_child(*shell, (*shell)->exit_status);
	if (!cmd->args || !cmd->args[0] || cmd->args[0][0] == '\0')
	{
		if (cmd->redir)
			exit_child(*shell, 0);
		ft_putendl_fd(": command not found", STDERR_FILENO);
		exit_child(*shell, 127);
	}
	if (is_builtin(cmd->args[0]))
	{
		builtin(cmd, *shell);
		exit_child(*shell, (*shell)->exit_status);
	}
	if (!cmd->path)
		child_no_path(cmd, *shell);
	else
		child_exec(cmd, *shell);
}
