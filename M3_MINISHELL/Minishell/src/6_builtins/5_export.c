/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   5_export.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 17:19:07 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 12:05:06 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	add_env(t_shell *shell, char *token)
{
	int		count;
	int		i;
	char	**tab_new_var;

	count = 0;
	while (shell->env[count])
		count++;
	tab_new_var = ft_gc_calloc(count + 2, sizeof(char *), &shell->gc_env);
	if (!tab_new_var)
		return (ERROR);
	i = 0;
	while (shell->env[i])
	{
		tab_new_var[i] = shell->env[i];
		i++;
	}
	tab_new_var[i] = ft_gc_strdup(token, &shell->gc_env);
	if (!tab_new_var[i])
		return (ERROR);
	shell->env = tab_new_var;
	return (SUCCESS);
}

int	update_env_export(t_shell *shell, const char *name, char *arg)
{
	int		i;
	int		name_len;
	char	*new_entry;

	name_len = ft_strlen(name);
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], name, name_len) == 0
			&& shell->env[i][name_len] == '=')
		{
			new_entry = ft_gc_strdup(arg, &shell->gc_env);
			if (!new_entry)
				return (ERROR);
			shell->env[i] = new_entry;
			return (SUCCESS);
		}
		i++;
	}
	return (ERROR);
}

static int	print_export(char **env_copy)
{
	int		i;
	char	*eq;

	i = 0;
	if (sort_env(env_copy) != SUCCESS)
		return (ERROR);
	while (env_copy[i])
	{
		eq = ft_strchr(env_copy[i], '=');
		if (eq)
		{
			*eq = '\0';
			printf("declare -x %s=\"%s\"\n", env_copy[i], eq + 1);
			*eq = '=';
		}
		else
			printf("declare -x %s\n", env_copy[i]);
		i++;
	}
	return (SUCCESS);
}

static int	process_arg(t_shell *shell, char *arg)
{
	if (ft_strchr(arg, '='))
		return (process_arg_with_equal(shell, arg));
	if (is_valid_name(arg) != SUCCESS)
		return (ERROR);
	if (update_env_export(shell, arg, arg) == ERROR)
		if (add_env(shell, arg) != SUCCESS)
			return (ERROR);
	return (SUCCESS);
}

int	ft_export(t_cmd *cmd, t_shell *shell)
{
	char	**env_copy;
	int		i;
	int		ret;

	i = 1;
	ret = SUCCESS;
	if (cmd->args[1] == NULL)
	{
		env_copy = copy_env(shell->env, shell);
		if (!env_copy)
			return (ERROR);
		return (print_export(env_copy));
	}
	while (cmd->args[i])
	{
		if (process_arg(shell, cmd->args[i]) != SUCCESS)
		{
			ft_putstr_fd("export: '", 2);
			ft_putstr_fd(cmd->args[i], 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			ret = ERROR;
		}
		i++;
	}
	return (ret);
}
