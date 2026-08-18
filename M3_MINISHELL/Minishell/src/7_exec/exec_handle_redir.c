/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_handle_redir.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 12:00:00 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 12:17:46 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	handle_redir_input(t_redir **redir, t_shell **shell, int *fd)
{
	if ((*redir)->type == TYPE_HEREDOC && (*redir)->heredoc_fd <= 0)
	{
		*redir = (*redir)->next;
		return (2);
	}
	if (redir_input(shell, *redir, fd) == ERROR)
		return (ERROR);
	return (SUCCESS);
}

static int	handle_redir_out(t_shell **shell, t_redir *redir, int *fd)
{
	if (redir_output(shell, redir, fd) == ERROR)
		return (ERROR);
	if (*fd > 0)
		close(*fd);
	return (SUCCESS);
}

int	handle_redir(t_cmd *cmd, t_shell **shell)
{
	t_redir	*redir;
	int		fd;
	int		ret;

	fd = 0;
	redir = cmd->redir;
	while (redir)
	{
		if (redir->type == TYPE_HEREDOC || redir->type == TYPE_INPUT)
		{
			ret = handle_redir_input(&redir, shell, &fd);
			if (ret == 2)
				continue ;
			if (ret == ERROR)
				return (ERROR);
		}
		else if (redir->type == TYPE_TRUNCATE || redir->type == TYPE_APPEND)
		{
			if (handle_redir_out(shell, redir, &fd) == ERROR)
				return (ERROR);
		}
		redir = redir->next;
	}
	return (SUCCESS);
}
