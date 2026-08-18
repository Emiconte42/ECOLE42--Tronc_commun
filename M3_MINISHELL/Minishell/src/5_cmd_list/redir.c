/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 15:44:43 by wihumeau          #+#    #+#             */
/*   Updated: 2026/06/14 13:49:55 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	init_redir(t_redir *redir)
{
	redir->file = NULL;
	redir->type = -1;
	redir->heredoc_fd = -1;
	redir->in_quote_flag = -1;
	redir->next = NULL;
	return (SUCCESS);
}

static char	*build_redir_file(t_token *token, t_shell *shell)
{
	t_token	*cur;
	char	*file;
	char	*joined;

	cur = token->next;
	file = ft_gc_strdup(cur->value, &shell->gc_loop);
	if (!file)
		return (NULL);
	cur = cur->next;
	while (cur && cur->is_joined)
	{
		joined = ft_gc_strjoin(file, cur->value, &shell->gc_loop);
		if (!joined)
			return (NULL);
		file = joined;
		cur = cur->next;
	}
	return (file);
}

int	assign_redir(t_redir *redir, t_token *token, t_shell *shell)
{
	redir->file = build_redir_file(token, shell);
	if (!redir->file)
		return (ERROR);
	if (token->type == TYPE_HEREDOC)
	{
		redir->type = TYPE_HEREDOC;
		redir->heredoc_fd = token->fd;
	}
	else if (token->value[0] == '<')
		redir->type = TYPE_INPUT;
	else if (token->value[0] == '>' && token->value[1]
		&& token->value[1] == '>')
		redir->type = TYPE_APPEND;
	else if (token->value[0] == '>')
		redir->type = TYPE_TRUNCATE;
	if (token->state == STATE_SIMPLE_QUOTE)
		redir->in_quote_flag = 1;
	else
		redir->in_quote_flag = 0;
	return (SUCCESS);
}

int	create_redir(t_shell *shell, t_token *token, t_cmd *cmd)
{
	t_redir	*new_redir;

	new_redir = ft_gc_calloc(1, sizeof(t_redir), &shell->gc_loop);
	if (!new_redir)
		return (ERROR);
	init_redir(new_redir);
	assign_redir(new_redir, token, shell);
	add_redir_back(cmd, new_redir);
	return (SUCCESS);
}
