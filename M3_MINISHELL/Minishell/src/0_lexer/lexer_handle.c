/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_handle.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 11:32:35 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/27 11:32:35 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	handle_normal(t_shell *shell, t_lexer *lexer)
{
	t_type	type;

	type = which_type(lexer->raw[lexer->i]);
	if (type == TYPE_SPACE || type == TYPE_PIPE)
		handle_normal_space_pipe(shell, type, lexer);
	else if (type == TYPE_REDIRECTION)
		handle_normal_redirection(shell, type, lexer);
	else if (type == TYPE_SIMPLE_QUOTE || type == TYPE_DOUBLE_QUOTE
		|| type == TYPE_OTHER)
		handle_normal_quotes_other(shell, type, lexer);
	return (SUCCESS);
}

int	handle_simple_quote(t_shell *shell, t_lexer *lexer)
{
	t_type	type;

	type = which_type(lexer->raw[lexer->i]);
	if (type == TYPE_SIMPLE_QUOTE)
	{
		lexer->i++;
		close_token(shell, lexer);
		lexer->start = -1;
		lexer->state_now = STATE_NORMAL;
		lexer->i--;
	}
	return (SUCCESS);
}

int	handle_double_quote(t_shell *shell, t_lexer *lexer)
{
	t_type	type;

	type = which_type(lexer->raw[lexer->i]);
	if (type == TYPE_DOUBLE_QUOTE)
	{
		lexer->i++;
		close_token(shell, lexer);
		lexer->start = -1;
		lexer->state_now = STATE_NORMAL;
		lexer->i--;
	}
	return (SUCCESS);
}

int	handle_dollar_quote(t_shell *shell, t_lexer *lexer)
{
	if (lexer->raw[lexer->i] == '\'')
	{
		close_token(shell, lexer);
		lexer->start = -1;
		lexer->state_now = STATE_NORMAL;
	}
	return (SUCCESS);
}
