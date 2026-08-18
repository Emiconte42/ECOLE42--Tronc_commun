/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_init.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 11:32:38 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/27 11:32:38 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	init_lexer(char *input, t_shell *shell)
{
	shell->lexer.raw = input;
	shell->lexer.i = 0;
	shell->lexer.start = -1;
	shell->lexer.state_now = STATE_NORMAL;
	shell->lexer.was_joined = 0;
	shell->lexer.head = NULL;
	return (SUCCESS);
}
