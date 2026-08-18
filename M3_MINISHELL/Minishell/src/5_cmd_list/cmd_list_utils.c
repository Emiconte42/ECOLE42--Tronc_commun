/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_list_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 09:50:47 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 16:27:36 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	add_cmd_back(t_shell *shell, t_cmd *new)
{
	t_cmd	*cur;

	if (shell->cmd == NULL)
	{
		shell->cmd = new;
		return (SUCCESS);
	}
	cur = shell->cmd;
	while (cur->next != NULL)
		cur = cur->next;
	cur->next = new;
	return (SUCCESS);
}

int	add_redir_back(t_cmd *cmd, t_redir *new)
{
	t_redir	*head;

	if (cmd->redir == NULL)
	{
		cmd->redir = new;
		return (SUCCESS);
	}
	head = cmd->redir;
	while (head->next != NULL)
		head = head->next;
	head->next = new;
	return (SUCCESS);
}

int	token_list_size(t_token *token)
{
	int		size;
	t_token	*count;

	size = 0;
	count = token;
	while (count && count->type != TYPE_PIPE)
	{
		if (count->state == STATE_NORMAL
			&& (count->type == TYPE_REDIRECTION || count->type == TYPE_HEREDOC))
			count = count->next;
		else
			size++;
		count = count->next;
	}
	return (size);
}

void	skip_token_joined(t_token **token)
{
	while ((*token)->next && (*token)->next->is_joined)
		*token = (*token)->next;
}
