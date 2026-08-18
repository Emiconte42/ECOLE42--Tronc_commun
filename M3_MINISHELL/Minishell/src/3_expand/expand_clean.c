/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_clean.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 09:35:41 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 11:25:20 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*trim_multi_spaces(char *var_value, int i, t_shell *shell)
{
	int		start;
	int		end;
	char	*first;
	char	*last;

	start = 0;
	end = i;
	while (var_value[i + 1] == 32 || var_value[i + 1] == 9)
		i++;
	if (var_value[i] == '\0')
		return (ft_gc_substr(var_value, start, end, &shell->gc_loop));
	first = ft_gc_substr(var_value, start, end, &shell->gc_loop);
	start = i;
	while (var_value[i])
		i++;
	end = i;
	last = ft_gc_substr(var_value, start, end, &shell->gc_loop);
	return (ft_gc_strjoin(first, last, &shell->gc_loop));
}

char	*ft_str_no_space_tab(char *var_value, t_shell *shell)
{
	int	i;

	i = 0;
	while (var_value[i])
	{
		if (var_value[i] == 32 || var_value[i] == 9)
		{
			if (var_value[i + 1] == 32 || var_value[i + 1] == 9)
				return (trim_multi_spaces(var_value, i, shell));
		}
		i++;
	}
	return (var_value);
}
