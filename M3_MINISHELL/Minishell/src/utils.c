/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 19:59:49 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 15:38:45 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	**copy_env(char **env, t_shell *shell)
{
	int		count;
	int		i;
	char	**copy;

	count = 0;
	while (env[count])
		count++;
	copy = ft_gc_calloc(count + 1, sizeof(char *), &shell->gc_env);
	if (!copy)
		return (NULL);
	i = 0;
	while (i < count)
	{
		copy[i] = ft_gc_strdup(env[i], &shell->gc_env);
		if (!copy[i])
			return (NULL);
		i++;
	}
	copy[i] = NULL;
	return (copy);
}

int	is_empty_line(char *line)
{
	int	i;

	i = 0;
	if (line[0] == '\\' && line[1] == '\0')
		return (ERROR);
	while (line[i])
	{
		if (line[i] != ' ' && line[i] != '\t')
			return (SUCCESS);
		i++;
	}
	return (ERROR);
}

static int	init_shlvl(t_shell *shell)
{
	char	*shlvl;

	if (!get_env_var(shell, "SHLVL"))
		return (add_env_cd(shell, "SHLVL", "1"));
	shlvl = get_env_var(shell, "SHLVL");
	return (update_env_cd(shell, "SHLVL",
			ft_gc_itoa(ft_atoi(shlvl) + 1, &shell->gc_env)));
}

int	init_min_env(t_shell *shell, char **argv)
{
	char	cwd[4096];

	if (!get_env_var(shell, "PWD"))
	{
		if (getcwd(cwd, sizeof(cwd)))
		{
			if (add_env_cd(shell, "PWD", cwd) != SUCCESS)
				return (ERROR);
		}
	}
	if (init_shlvl(shell) != SUCCESS)
		return (ERROR);
	if (!get_env_var(shell, "_"))
	{
		if (add_env_cd(shell, "_", argv[0]) != SUCCESS)
			return (ERROR);
	}
	return (SUCCESS);
}

void	handle_exit(t_shell *shell, char *line)
{
	write(1, "exit\n", 5);
	if (shell->stdin != -1)
		close(shell->stdin);
	if (shell->stdout != -1)
		close(shell->stdout);
	clear_history();
	ft_free_all(shell, line);
	ft_gc_free_env(shell);
	exit(shell->exit_status);
}
