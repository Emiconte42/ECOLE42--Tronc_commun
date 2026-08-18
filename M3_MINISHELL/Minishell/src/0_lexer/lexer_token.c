/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/28 14:56:21 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/28 14:56:21 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

t_type	which_type(char c)
{
	if (c == ' ' || c == '\t')
		return (TYPE_SPACE);
	else if (c == '|')
		return (TYPE_PIPE);
	else if (c == '<' || c == '>')
		return (TYPE_REDIRECTION);
	else if (c == '\'')
		return (TYPE_SIMPLE_QUOTE);
	else if (c == '"')
		return (TYPE_DOUBLE_QUOTE);
	return (TYPE_OTHER);
}

static void	add_token_back(t_lexer *lexer, t_token *new)
{
	t_token	*current;

	if (lexer->head == NULL)
	{
		lexer->head = new;
		return ;
	}
	current = lexer->head;
	while (current->next != NULL)
		current = current->next;
	current->next = new;
}

int	close_token(t_shell *shell, t_lexer *lexer)
{
	t_token	*token;

	token = ft_gc_calloc(1, sizeof(t_token), &shell->gc_loop);
	if (!token)
		return (perror("malloc"), ERROR);
	token->value = ft_gc_substr(lexer->raw, lexer->start, lexer->i
			- lexer->start, &shell->gc_loop);
	if (!token->value)
		return (perror("malloc"), ERROR);
	token->len_value = ft_strlen(token->value);
	if (lexer->state_now == STATE_DOUBLE_QUOTE)
		token->type = TYPE_DOUBLE_QUOTE;
	else if (lexer->state_now == STATE_SIMPLE_QUOTE)
		token->type = TYPE_SIMPLE_QUOTE;
	else if (lexer->state_now == STATE_DOLLAR_QUOTE)
		token->type = TYPE_SIMPLE_QUOTE;
	else
		token->type = TYPE_OTHER;
	token->state = lexer->state_now;
	token->is_joined = lexer->was_joined;
	token->next = NULL;
	add_token_back(lexer, token);
	lexer->was_joined = 1;
	return (SUCCESS);
}

int	create_token(t_shell *shell, t_lexer *lexer, t_type type)
{
	t_token	*new;

	new = ft_gc_calloc(1, sizeof(t_token), &shell->gc_loop);
	if (!new)
		return (ERROR);
	new->value = ft_gc_substr(lexer->raw, lexer->i, 1, &shell->gc_loop);
	if (!new->value)
		return (ERROR);
	new->len_value = 1;
	new->type = type;
	new->state = STATE_NORMAL;
	new->is_joined = 0;
	new->next = NULL;
	add_token_back(lexer, new);
	return (SUCCESS);
}

int	create_token_double_redir(t_shell *shell, t_lexer *lexer, t_type type)
{
	t_token	*new;

	new = ft_gc_calloc(1, sizeof(t_token), &shell->gc_loop);
	if (!new)
		return (ERROR);
	new->value = ft_gc_substr(lexer->raw, lexer->i, 2, &shell->gc_loop);
	if (!new->value)
		return (ERROR);
	new->len_value = 2;
	new->type = type;
	new->state = STATE_NORMAL;
	new->is_joined = 0;
	new->next = NULL;
	add_token_back(lexer, new);
	return (SUCCESS);
}
