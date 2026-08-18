/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 17:30:31 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 11:25:43 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	is_word_char(char c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
		|| (c >= '0' && c <= '9') || c == '_')
		return (SUCCESS);
	return (ERROR);
}

char	*get_env_value(t_shell *shell, t_expand *expand, char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		if ((ft_strncmp(env[i], expand->var_name, expand->len_name) == SUCCESS)
			&& env[i][expand->len_name] == '=')
			return (ft_gc_strdup(&env[i][expand->len_name + 1],
				&shell->gc_loop));
		i++;
	}
	return (NULL);
}

void	remove_token(t_token *cur, t_token *prev, t_shell *shell)
{
	if (cur == prev)
	{
		shell->lexer.head = cur->next;
	}
	else if (cur->next == NULL)
	{
		cur = prev;
		cur->next = NULL;
	}
	else
	{
		cur = prev;
		cur->next = cur->next->next;
	}
}

void	join_skip_chars(t_token *cur, t_shell *shell, int i, int j)
{
	char	*first;
	char	*last;

	first = ft_gc_substr(cur->value, 0, i, &shell->gc_loop);
	last = ft_gc_substr(cur->value, i + j, cur->len_value, &shell->gc_loop);
	cur->value = ft_gc_strjoin(first, last, &shell->gc_loop);
	cur->len_value = ft_strlen(cur->value);
	i--;
}

int	check_expand_value(t_shell *shell, t_expand *expand)
{
	int	i;

	i = 0;
	while (expand->var_value[i])
	{
		if (expand->var_value[i] == 32 || expand->var_value[i] == 9)
		{
			if (expand->var_value[i + 1] == 32
				|| expand->var_value[i + 1] == 9)
			{
				expand->var_value = ft_str_no_space_tab(expand->var_value,
						shell);
				if (expand->var_value == NULL)
					return (ERROR);
				i = 0;
				continue ;
			}
		}
		i++;
	}
	return (SUCCESS);
}
