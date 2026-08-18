/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   close_fd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 11:00:06 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 17:50:40 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	invalidate_redir_fd(t_cmd *cmd, int fd)
{
	t_redir	*redir;

	while (cmd)
	{
		redir = cmd->redir;
		while (redir)
		{
			if (redir->heredoc_fd == fd)
				redir->heredoc_fd = -1;
			redir = redir->next;
		}
		cmd = cmd->next;
	}
}

void	close_heredoc_fds(t_shell *shell)
{
	t_token	*token;

	token = shell->lexer.head;
	while (token)
	{
		if (token->type == TYPE_HEREDOC && token->fd > 0)
		{
			invalidate_redir_fd(shell->cmd, token->fd);
			if (token->fd != -1)
				close(token->fd);
			token->fd = -1;
		}
		token = token->next;
	}
}
