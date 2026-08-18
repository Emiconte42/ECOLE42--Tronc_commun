/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 17:37:34 by wissalhumea       #+#    #+#             */
/*   Updated: 2026/06/14 12:00:56 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	check_slash_prefix(t_cmd *cmd, char *slash)
{
	struct stat	st;
	char		*prefix;
	size_t		len;

	while (slash)
	{
		len = slash - cmd->args[0];
		prefix = ft_substr(cmd->args[0], 0, len);
		if (!prefix)
			return (ERROR);
		if (stat(prefix, &st) == 0 && !S_ISDIR(st.st_mode))
		{
			free(prefix);
			cmd->perm_denied = 1;
			cmd->is_dir = 0;
			return (ERROR);
		}
		free(prefix);
		slash = ft_strchr(slash + 1, '/');
	}
	return (SUCCESS);
}

static char	*check_slash_path(t_cmd *cmd)
{
	struct stat	st;
	char		*slash;

	slash = ft_strchr(cmd->args[0] + 1, '/');
	if (check_slash_prefix(cmd, slash) != SUCCESS)
		return (NULL);
	if (access(cmd->args[0], F_OK) != 0)
	{
		cmd->not_found = 1;
		return (NULL);
	}
	if (stat(cmd->args[0], &st) == 0 && S_ISDIR(st.st_mode))
	{
		cmd->is_dir = 1;
		cmd->perm_denied = 1;
		return (NULL);
	}
	if (access(cmd->args[0], X_OK) == 0)
		return (cmd->args[0]);
	cmd->perm_denied = 1;
	return (NULL);
}

char	*find_path(t_shell *shell, t_cmd *cmd)
{
	int		i;
	char	**path_tab;
	char	*complete_cmd;
	char	*cmd_path;

	if (!cmd)
		return (NULL);
	i = 0;
	path_tab = isolate_path(shell, cmd);
	if (!path_tab)
		return (NULL);
	complete_cmd = ft_gc_strjoin("/", cmd->args[0], &shell->gc_loop);
	while (path_tab[i])
	{
		cmd_path = ft_gc_strjoin(path_tab[i], complete_cmd, &shell->gc_loop);
		if (access(cmd_path, F_OK | X_OK) == 0)
			return (cmd_path);
		i++;
	}
	return (NULL);
}

char	*assign_path(t_shell *shell, t_cmd *cmd)
{
	if (cmd->args[0] == NULL)
		return (NULL);
	if (cmd->args[0][0] == '\\' && cmd->args[0][1] != '\0')
		cmd->args[0] = cmd->args[0] + 1;
	if (is_builtin(cmd->args[0]))
		return (NULL);
	if (ft_strchr(cmd->args[0], '/'))
		return (check_slash_path(cmd));
	return (find_path(shell, cmd));
}
