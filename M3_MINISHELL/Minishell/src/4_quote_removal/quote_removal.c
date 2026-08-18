/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_removal.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 15:24:08 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 11:37:46 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	handle_quote_state(char c, int *state, char *new_value, int *j)
{
	if (*state == STATE_NORMAL)
	{
		if (c == '\'')
			*state = STATE_SIMPLE_QUOTE;
		else if (c == '"')
			*state = STATE_DOUBLE_QUOTE;
		else
			new_value[(*j)++] = c;
	}
	else if (*state == STATE_SIMPLE_QUOTE)
	{
		if (c == '\'')
			*state = STATE_NORMAL;
		else
			new_value[(*j)++] = c;
	}
	else if (*state == STATE_DOUBLE_QUOTE)
	{
		if (c == '"')
			*state = STATE_NORMAL;
		else
			new_value[(*j)++] = c;
	}
}

static int	skip_dollar_quote(t_token *cur, int *i)
{
	if (cur->value[*i] == '$' && cur->value[*i + 1] == '"')
	{
		(*i)++;
		return (1);
	}
	return (0);
}

static void	remove_quotes_from_token(t_shell *shell, t_token *cur)
{
	char	*new_value;
	int		state;
	int		i;
	int		j;

	if (!cur || !cur->value)
		return ;
	state = STATE_NORMAL;
	i = 0;
	j = 0;
	new_value = ft_gc_calloc(1, sizeof(char) * (ft_strlen(cur->value) + 1),
			&shell->gc_loop);
	if (!new_value)
		return ;
	while (cur->value[i])
	{
		if (state == STATE_NORMAL && skip_dollar_quote(cur, &i))
			continue ;
		handle_quote_state(cur->value[i], &state, new_value, &j);
		i++;
	}
	new_value[j] = '\0';
	cur->value = new_value;
	cur->len_value = ft_strlen(new_value);
}

int	quote_removal(t_shell *shell)
{
	t_token	*cur;

	cur = shell->lexer.head;
	while (cur)
	{
		if (cur->type != TYPE_PIPE && cur->type != TYPE_REDIRECTION
			&& cur->type != TYPE_HEREDOC)
			remove_quotes_from_token(shell, cur);
		cur = cur->next;
	}
	return (SUCCESS);
}
