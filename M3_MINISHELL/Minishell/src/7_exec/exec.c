/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 11:27:29 by wihumeau          #+#    #+#             */
/*   Updated: 2026/06/14 12:17:14 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	waiting(t_shell *shell)
{
	t_cmd	*head;
	int		status;

	head = shell->cmd;
	status = shell->exit_status;
	if (!head)
		return (ERROR);
	while (head)
	{
		waitpid(head->pid, &status, 0);
		if (!head->next)
		{
			if (WIFEXITED(status))
				shell->exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				shell->exit_status = 128 + WTERMSIG(status);
		}
		head = head->next;
	}
	return (SUCCESS);
}

static void	exec_signals(t_shell *shell)
{
	if (g_signal == SIGINT)
		shell->exit_status = 130;
	else if (g_signal == SIGQUIT)
		shell->exit_status = 131;
	g_signal = 0;
}

static int	exec_builtin(t_cmd *cmd, t_shell **shell)
{
	if (cmd->redir && handle_redir(cmd, shell) == ERROR)
	{
		close_heredoc_fds(*shell);
		return (close_and_dup_std(shell, SUCCESS));
	}
	builtin(cmd, *shell);
	close_and_dup_std(shell, 0);
	close_heredoc_fds(*shell);
	return (SUCCESS);
}

static int	parent(t_cmd *cmd, t_shell **shell)
{
	int	fd[2];

	while (cmd)
	{
		if (cmd->next && pipe(fd) < 0)
			return (perror("pipe"), ERROR);
		cmd->pid = fork();
		if (cmd->pid < 0)
			return (perror("fork"), close(fd[0]), close(fd[1]), ERROR);
		if (cmd->pid == 0)
			child(cmd, fd, shell);
		if (cmd->next)
		{
			if (close(fd[1]) < 0 || dup2(fd[0], STDIN_FILENO) < 0
				|| close(fd[0]) < 0)
				return (perror("close or dup stdin "), ERROR);
		}
		cmd = cmd->next;
	}
	return (SUCCESS);
}

int	exec(t_shell *shell)
{
	t_cmd	*cmd;

	shell->stdin = dup(STDIN_FILENO);
	shell->stdout = dup(STDOUT_FILENO);
	cmd = shell->cmd;
	if (!cmd)
		return (close_std(&shell, ERROR));
	if (!cmd->next && cmd->args && cmd->args[0] && is_builtin(cmd->args[0]))
		exec_builtin(cmd, &shell);
	else
	{
		set_signals(SIG_EXEC);
		if (parent(cmd, &shell) == ERROR)
			return (close_std(&shell, ERROR));
		close_and_dup_std(&shell, 0);
		close_heredoc_fds(shell);
		if (waiting(shell) == ERROR)
			return (ERROR);
	}
	exec_signals(shell);
	return (SUCCESS);
}
