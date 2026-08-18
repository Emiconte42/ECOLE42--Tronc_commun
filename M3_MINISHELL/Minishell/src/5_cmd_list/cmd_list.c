/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_list.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 17:20:36 by wihumeau          #+#    #+#             */
/*   Updated: 2026/06/14 16:29:48 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	list_cmd(t_shell *shell)
{
	t_token	*token;

	token = shell->lexer.head;
	while (token != NULL)
	{
		create_cmd(shell, &token);
		if ((token && token->next == NULL) || !token)
			return (SUCCESS);
		token = token->next;
	}
	return (SUCCESS);
}

int	create_cmd(t_shell *shell, t_token **token)
{
	t_cmd	*new_cmd;

	new_cmd = ft_gc_calloc(1, sizeof(t_cmd), &shell->gc_loop);
	if (!new_cmd)
		return (ERROR);
	init_cmd(new_cmd, shell);
	assign_cmd(new_cmd, token, shell);
	add_cmd_back(shell, new_cmd);
	return (SUCCESS);
}

int	init_cmd(t_cmd *cmd, t_shell *shell)
{
	cmd->args = NULL;
	cmd->path = NULL;
	cmd->fdin = STDIN_FILENO;
	cmd->fdout = STDOUT_FILENO;
	cmd->pid = -1;
	cmd->env = shell->env;
	cmd->redir = NULL;
	cmd->next = NULL;
	return (SUCCESS);
}

static int	assign_arg(t_cmd *cmd, t_token **token, t_shell *shell, int *i)
{
	char	*joined;

	if ((*token)->is_joined && *i > 0)
	{
		joined = ft_gc_strjoin(cmd->args[*i - 1],
				(*token)->value, &shell->gc_loop);
		if (!joined)
			return (ERROR);
		cmd->args[*i - 1] = joined;
	}
	else
	{
		cmd->args[*i] = ft_gc_strdup((*token)->value, &shell->gc_loop);
		if (!cmd->args[*i])
			return (ERROR);
		(*i)++;
	}
	return (SUCCESS);
}

int	assign_cmd(t_cmd *cmd, t_token **token, t_shell *shell)
{
	int		i;

	i = 0;
	shell->cmd_list_size = token_list_size(*token);
	cmd->args = ft_gc_calloc(1, sizeof(char *)
			* (shell->cmd_list_size + 1), &shell->gc_loop);
	if (!cmd->args)
		return (ERROR);
	while (*token && (*token)->type != TYPE_PIPE)
	{
		if ((*token)->state == STATE_NORMAL && ((*token)->type
				== TYPE_REDIRECTION || (*token)->type == TYPE_HEREDOC))
		{
			create_redir(shell, *token, cmd);
			if (!(*token)->next)
				break ;
			*token = (*token)->next;
			skip_token_joined(token);
		}
		else
			assign_arg(cmd, token, shell, &i);
		*token = (*token)->next;
	}
	cmd->path = assign_path(shell, cmd);
	return (SUCCESS);
}
