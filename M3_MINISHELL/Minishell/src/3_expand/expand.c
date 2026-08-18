/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 17:30:51 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/15 11:58:39 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*rebuild_token(t_shell *shell, int i, t_expand *expand,
				t_token *cur)
{
	char	*first_part;
	char	*last_part;
	char	*new_value;

	first_part = ft_gc_substr(cur->value, 0, i, &shell->gc_loop);
	if (cur->value[i + expand->len_name + 1] != '\0')
		last_part = ft_gc_substr(cur->value, i + expand->len_name + 1,
				cur->len_value, &shell->gc_loop);
	else
		last_part = ft_gc_strdup("", &shell->gc_loop);
	new_value = ft_gc_strjoin(first_part, expand->var_value, &shell->gc_loop);
	if (!new_value)
		return (NULL);
	new_value = ft_gc_strjoin(new_value, last_part, &shell->gc_loop);
	if (!new_value)
		return (NULL);
	cur->len_value = ft_strlen(new_value);
	return (new_value);
}

static char	*expand_exit_code(t_shell *shell, t_token *cur, int i)
{
	char	*first_part;
	char	*value;
	char	*last_part;
	char	*new_value;

	first_part = ft_gc_substr(cur->value, 0, i, &shell->gc_loop);
	value = ft_gc_itoa(shell->exit_status, &shell->gc_loop);
	last_part = ft_gc_substr(cur->value, i + 2, cur->len_value,
			&shell->gc_loop);
	new_value = ft_gc_strjoin(first_part, value, &shell->gc_loop);
	if (!new_value)
		return (NULL);
	new_value = ft_gc_strjoin(new_value, last_part, &shell->gc_loop);
	if (!new_value)
		return (NULL);
	cur->len_value = ft_strlen(new_value);
	cur->value = new_value;
	return (cur->value);
}

static char	*expand_var(t_token *cur, t_shell *shell, int *i)
{
	t_expand	*expand;

	expand = &shell->expand;
	expand->len_name = 0;
	*i += 1;
	while (cur->value[*i + expand->len_name]
		&& is_word_char(cur->value[*i + expand->len_name]) == SUCCESS)
		expand->len_name++;
	expand->var_name = ft_gc_substr(cur->value, *i,
			expand->len_name, &shell->gc_loop);
	expand->var_value = get_env_value(shell, expand, shell->env);
	if (expand->var_value == NULL)
		expand->var_value = ft_gc_strdup("", &shell->gc_loop);
	expand->len_var_value = ft_strlen(expand->var_value);
	check_expand_value(shell, expand);
	expand->len_var_value = ft_strlen(expand->var_value);
	cur->value = rebuild_token(shell, *i - 1, expand, cur);
	*i = (*i - 1) + expand->len_var_value - 1;
	return (cur->value);
}

static int	handle_dollar(t_token *cur, t_shell *shell, int *i)
{
	if (cur->value[*i + 1] == '?')
		cur->value = expand_exit_code(shell, cur, *i);
	else if (ft_isdigit(cur->value[*i + 1]))
		join_skip_chars(cur, shell, *i, 2);
	else if (cur->value[*i + 1] == '\'')
		join_skip_chars(cur, shell, *i, 1);
	else if (is_word_char(cur->value[*i + 1]) == SUCCESS)
	{
		cur->value = expand_var(cur, shell, i);
		(*i)++;
		return (1);
	}
	else
		return ((*i)++, 1);
	*i = 0;
	return (0);
}

int	expand_value(t_token *cur, t_shell *shell)
{
	int	i;

	i = 0;
	while (cur->value[i])
	{
		if (cur->value[i] == '$')
		{
			if (handle_dollar(cur, shell, &i))
				continue ;
			continue ;
		}
		i++;
	}
	return (SUCCESS);
}
