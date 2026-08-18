/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_cd_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 17:18:51 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/11 18:12:47 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	add_env_cd(t_shell *shell, const char *key, const char *value)
{
	int		count;
	int		i;
	char	**new_env;
	char	*tmp;
	char	*entry;

	count = 0;
	while (shell->env[count])
		count++;
	new_env = ft_gc_calloc(count + 2, sizeof(char *), &shell->gc_env);
	if (!new_env)
		return (ERROR);
	i = 0;
	while (shell->env[i])
	{
		new_env[i] = shell->env[i];
		i++;
	}
	tmp = ft_gc_strjoin(key, "=", &shell->gc_env);
	entry = ft_gc_strjoin(tmp, value, &shell->gc_env);
	new_env[i] = entry;
	shell->env = new_env;
	return (SUCCESS);
}

int	update_env_cd(t_shell *shell, const char *key, const char *value)
{
	int		i;
	int		key_len;
	char	*new_entry;
	char	*tmp;

	key_len = ft_strlen(key);
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], key, key_len) == 0
			&& shell->env[i][key_len] == '=')
		{
			tmp = ft_gc_strjoin(key, "=", &shell->gc_env);
			new_entry = ft_gc_strjoin(tmp, value, &shell->gc_env);
			shell->env[i] = new_entry;
			return (SUCCESS);
		}
		i++;
	}
	return (ERROR);
}

char	*get_env_var(t_shell *shell, const char *key)
{
	int	i;
	int	key_len;

	key_len = ft_strlen(key);
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], key, key_len) == 0
			&& shell->env[i][key_len] == '=')
			return (shell->env[i] + key_len + 1);
		i++;
	}
	return (NULL);
}
