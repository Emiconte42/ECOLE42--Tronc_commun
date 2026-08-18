/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 12:00:00 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 12:01:07 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	**isolate_path(t_shell *shell, t_cmd *cmd)
{
	int		i;
	char	**path_variable;
	char	**path_tab;

	i = 0;
	while (cmd->env[i])
	{
		if (ft_strncmp(cmd->env[i], "PATH=", 5) == 0)
		{
			path_variable = ft_gc_split(cmd->env[i], '=', &shell->gc_loop);
			if (path_variable == NULL)
				return (NULL);
			path_tab = ft_gc_split(path_variable[1], ':', &shell->gc_loop);
			if (path_tab == NULL)
				return (NULL);
			return (path_tab);
		}
		i++;
	}
	return (NULL);
}

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "env") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}
