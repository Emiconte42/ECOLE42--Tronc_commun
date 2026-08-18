/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_cd.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 17:18:51 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 09:48:58 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*get_target_path(t_shell *shell, char *arg)
{
	char	*path;

	if (arg == NULL)
	{
		path = get_env_var(shell, "HOME");
		if (path == NULL)
			ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO);
		return (path);
	}
	if (ft_strcmp(arg, "-") == 0)
	{
		path = get_env_var(shell, "OLDPWD");
		if (path == NULL)
			ft_putstr_fd("cd: OLDPWD not set\n", STDERR_FILENO);
		else
		{
			ft_putstr_fd(path, STDOUT_FILENO);
			ft_putchar_fd('\n', STDOUT_FILENO);
		}
		return (path);
	}
	return (arg);
}

static int	update_pwd_vars(t_shell *shell, const char *old_pwd)
{
	char	*tmp;

	if (!old_pwd)
		return (ERROR);
	if (update_env_cd(shell, "OLDPWD", old_pwd) != SUCCESS)
		add_env_cd(shell, "OLDPWD", old_pwd);
	tmp = getcwd(NULL, 0);
	if (!tmp)
		return (perror("getcwd"), ERROR);
	if (update_env_cd(shell, "PWD", tmp) != SUCCESS)
		add_env_cd(shell, "PWD", tmp);
	free(tmp);
	return (SUCCESS);
}

int	ft_cd(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	*old_pwd;
	char	*tmp;

	if (cmd->args[1] && cmd->args[2])
	{
		ft_putstr_fd("cd: too many arguments\n", STDERR_FILENO);
		return (ERROR);
	}
	path = get_target_path(shell, cmd->args[1]);
	if (path == NULL)
		return (ERROR);
	tmp = getcwd(NULL, 0);
	if (!tmp)
		return (perror("getcwd"), ERROR);
	old_pwd = ft_gc_strdup(tmp, &shell->gc_loop);
	free(tmp);
	if (chdir(path) != 0)
	{
		ft_putstr_fd("cd: ", STDERR_FILENO);
		perror(path);
		return (ERROR);
	}
	return (update_pwd_vars(shell, old_pwd));
}
