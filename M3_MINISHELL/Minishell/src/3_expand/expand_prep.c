/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_prep.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 09:35:41 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 18:00:34 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	skip_final_dollar(t_token *cur, t_shell *shell)
{
	int	len;

	len = cur->len_value;
	if (len > 0 && cur->value[len - 1] == '$'
		&& cur->next && cur->next->is_joined
		&& cur->next->type == TYPE_SIMPLE_QUOTE)
	{
		cur->value = ft_gc_substr(cur->value, 0, len - 1, &shell->gc_loop);
		cur->len_value = len - 1;
	}
}

int	prep_expand(t_shell *shell)
{
	t_token	*cur;
	t_token	*prev;

	cur = shell->lexer.head;
	prev = shell->lexer.head;
	while (cur)
	{
		if (cur->type != TYPE_SIMPLE_QUOTE)
		{
			expand_value(cur, shell);
			skip_final_dollar(cur, shell);
			if (cur->value[0] == '\0')
			{
				remove_token(cur, prev, shell);
				cur = prev->next;
				continue ;
			}
		}
		prev = cur;
		cur = cur->next;
	}
	return (SUCCESS);
}
