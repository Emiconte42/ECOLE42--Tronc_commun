/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 10:41:46 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/24 10:41:46 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	start_lexer(char *input, t_shell *shell)
{
	t_lexer	*lexer;

	if (init_lexer(input, shell) != SUCCESS)
		return (ERROR);
	lexer = &shell->lexer;
	while (lexer->raw[lexer->i])
	{
		if (lexer->state_now == STATE_NORMAL)
			handle_normal(shell, lexer);
		else if (lexer->state_now == STATE_SIMPLE_QUOTE)
			handle_simple_quote(shell, lexer);
		else if (lexer->state_now == STATE_DOUBLE_QUOTE)
			handle_double_quote(shell, lexer);
		else if (lexer->state_now == STATE_DOLLAR_QUOTE)
			handle_dollar_quote(shell, lexer);
		lexer->i++;
	}
	if (lexer->state_now != STATE_NORMAL)
		return (syntax_error(shell, "unexpected EOF"));
	if (lexer->start != -1)
		close_token(shell, lexer);
	return (SUCCESS);
}
