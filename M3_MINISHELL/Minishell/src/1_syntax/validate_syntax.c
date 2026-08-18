/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_syntax.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 16:54:52 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/29 16:54:52 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	syntax_error(t_shell *shell, char *msg)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(msg, 2);
	ft_putendl_fd("'", 2);
	shell->exit_status = 2;
	return (ERROR);
}

static int	check_redir_syntax(t_shell *shell, t_token *cur)
{
	if (cur->next == NULL)
		return (syntax_error(shell, "newline"));
	if (cur->next->type == TYPE_REDIRECTION
		|| cur->next->type == TYPE_HEREDOC)
		return (syntax_error(shell, cur->next->value));
	if (cur->next->type == TYPE_PIPE)
		return (syntax_error(shell, "|"));
	return (SUCCESS);
}

int	validate_syntax(t_shell *shell)
{
	t_token	*cur;

	cur = shell->lexer.head;
	if (cur && cur->type == TYPE_PIPE)
		return (syntax_error(shell, "|"));
	if (cur && cur->next == NULL && cur->type == TYPE_OTHER
		&& ft_strcmp(cur->value, "!") == 0)
		return (shell->exit_status = 1, ERROR);
	while (cur)
	{
		if (cur->type == TYPE_PIPE && cur->next == NULL)
			return (syntax_error(shell, "|"));
		if (cur->type == TYPE_PIPE && cur->next
			&& cur->next->type == TYPE_PIPE)
			return (syntax_error(shell, "||"));
		if (cur->type == TYPE_REDIRECTION || cur->type == TYPE_HEREDOC)
		{
			if (check_redir_syntax(shell, cur) != SUCCESS)
				return (ERROR);
		}
		cur = cur->next;
	}
	return (SUCCESS);
}
