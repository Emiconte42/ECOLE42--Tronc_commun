/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 12:00:00 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 17:56:18 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static t_redir	*find_last_heredoc(t_cmd *cmd)
{
	t_redir	*redir;
	t_redir	*last;

	last = NULL;
	redir = cmd->redir;
	while (redir)
	{
		if (redir->type == TYPE_HEREDOC && redir->heredoc_fd > 0)
			last = redir;
		redir = redir->next;
	}
	return (last);
}

static void	close_one_heredoc_redir(t_shell *shell, t_cmd *cmd,
				t_redir *last_heredoc)
{
	t_redir	*redir;
	t_token	*token;

	redir = cmd->redir;
	while (redir)
	{
		if (redir->type == TYPE_HEREDOC && redir->heredoc_fd > 0
			&& redir != last_heredoc)
		{
			close(redir->heredoc_fd);
			token = shell->lexer.head;
			while (token)
			{
				if (token->fd == redir->heredoc_fd)
					token->fd = -1;
				token = token->next;
			}
			redir->heredoc_fd = -1;
		}
		redir = redir->next;
	}
}

int	redir_output(t_shell **shell, t_redir *redir, int *fd)
{
	if (redir->type == TYPE_TRUNCATE)
	{
		*fd = open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (*fd < 0)
			return (return_redir(shell, redir->file, 1));
	}
	else if (redir->type == TYPE_APPEND)
	{
		*fd = open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (*fd < 0)
			return (return_redir(shell, redir->file, 1));
	}
	if (*fd >= 0 && dup2(*fd, STDOUT_FILENO) < 0)
	{
		perror("dup2");
		close(*fd);
		return (ERROR);
	}
	return (SUCCESS);
}

int	redir_input(t_shell **shell, t_redir *redir, int *fd)
{
	if (redir->type == TYPE_HEREDOC)
	{
		*fd = redir->heredoc_fd;
		redir->heredoc_fd = -1;
	}
	else if (redir->type == TYPE_INPUT)
	{
		*fd = open(redir->file, O_RDONLY);
		if (*fd < 0)
			return (return_redir(shell, redir->file, 1));
	}
	if (dup2(*fd, STDIN_FILENO) < 0)
	{
		perror("dup input fd ");
		if (redir->heredoc_fd != -1)
			close(redir->heredoc_fd);
		return (redir->heredoc_fd = -1, ERROR);
	}
	if (*fd != STDIN_FILENO)
	{
		if (redir->heredoc_fd != -1)
			close(redir->heredoc_fd);
		redir->heredoc_fd = -1;
	}
	return (SUCCESS);
}

void	close_unused_heredoc_redirs(t_shell *shell, t_cmd *cur_cmd)
{
	t_cmd	*cmd;
	t_redir	*last_heredoc;

	cmd = shell->cmd;
	while (cmd)
	{
		last_heredoc = NULL;
		if (cmd == cur_cmd)
			last_heredoc = find_last_heredoc(cmd);
		close_one_heredoc_redir(shell, cmd, last_heredoc);
		cmd = cmd->next;
	}
}
