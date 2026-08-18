/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 13:43:25 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/27 13:43:25 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	handle_double_redir(t_shell *shell, t_lexer *lexer)
{
	if (lexer->raw[lexer->i + 1] == lexer->raw[lexer->i])
	{
		if (lexer->raw[lexer->i] == '>')
			create_token_double_redir(shell, lexer, TYPE_REDIRECTION);
		else if (lexer->raw[lexer->i] == '<')
			create_token_double_redir(shell, lexer, TYPE_HEREDOC);
		lexer->i++;
	}
	else if (lexer->raw[lexer->i] == '<'
		&& lexer->raw[lexer->i + 1] == '>')
	{
		create_token_double_redir(shell, lexer, TYPE_REDIRECTION);
		lexer->i++;
	}
	else
		create_token(shell, lexer, TYPE_REDIRECTION);
}

static void	handle_dollar_quote_start(t_shell *shell, t_lexer *lexer)
{
	if (lexer->start != -1)
		close_token(shell, lexer);
	lexer->start = -1;
	lexer->i++;
	lexer->start = lexer->i + 1;
	lexer->state_now = STATE_DOLLAR_QUOTE;
}

void	handle_normal_space_pipe(t_shell *shell, t_type type, t_lexer *lexer)
{
	if (type == TYPE_SPACE)
	{
		if (lexer->start != -1)
		{
			close_token(shell, lexer);
			lexer->start = -1;
		}
		lexer->was_joined = 0;
	}
	else if (type == TYPE_PIPE)
	{
		if (lexer->start != -1)
		{
			close_token(shell, lexer);
			lexer->start = -1;
		}
		lexer->was_joined = 0;
		create_token(shell, lexer, TYPE_PIPE);
	}
}

void	handle_normal_redirection(t_shell *shell, t_type type, t_lexer *lexer)
{
	if (type == TYPE_REDIRECTION)
	{
		if (lexer->start != -1)
		{
			close_token(shell, lexer);
			lexer->start = -1;
		}
		lexer->was_joined = 0;
		handle_double_redir(shell, lexer);
	}
}

void	handle_normal_quotes_other(t_shell *shell, t_type type, t_lexer *lexer)
{
	if (type == TYPE_SIMPLE_QUOTE)
	{
		if (lexer->start == -1)
			lexer->start = lexer->i;
		lexer->state_now = STATE_SIMPLE_QUOTE;
	}
	else if (type == TYPE_DOUBLE_QUOTE)
	{
		if (lexer->start == -1)
			lexer->start = lexer->i;
		lexer->state_now = STATE_DOUBLE_QUOTE;
	}
	else if (type == TYPE_OTHER)
	{
		if (lexer->raw[lexer->i] == '$' && lexer->raw[lexer->i + 1] == '\'')
			handle_dollar_quote_start(shell, lexer);
		else
		{
			if (lexer->start == -1)
				lexer->start = lexer->i;
		}
	}
}
