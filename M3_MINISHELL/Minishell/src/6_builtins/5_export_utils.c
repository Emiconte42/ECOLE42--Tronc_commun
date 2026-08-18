/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   5_export_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 13:31:17 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 12:04:34 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	is_valid_name(char *str)
{
	int	i;

	i = 0;
	if (str[i] != '_' && !ft_isalpha(str[i]))
		return (ERROR);
	else
		i++;
	while (str[i])
	{
		if ((str[i] != '_') && !ft_isalpha(str[i]) && !ft_isalnum(str[i]))
			return (ERROR);
		i++;
	}
	return (SUCCESS);
}

int	sort_env(char **env_copy)
{
	int		i;
	int		finish;
	char	*tmp;

	finish = 0;
	while (!finish)
	{
		finish = 1;
		i = 0;
		while (env_copy[i + 1])
		{
			if (ft_strcmp(env_copy[i], env_copy[i + 1]) > 0)
			{
				tmp = env_copy[i];
				env_copy[i] = env_copy[i + 1];
				env_copy[i + 1] = tmp;
				finish = 0;
			}
			i++;
		}
	}
	return (SUCCESS);
}

int	count_before_equal(char *str)
{
	int	count;

	count = 0;
	while (str[count] != '=' && str[count] != '\0')
		count++;
	return (count);
}

int	process_arg_with_equal(t_shell *shell, char *arg)
{
	char	*name;
	int		count;

	count = count_before_equal(arg);
	name = ft_gc_substr(arg, 0, count, &shell->gc_loop);
	if (name && is_valid_name(name) == SUCCESS)
	{
		if (update_env_export(shell, name, arg) != SUCCESS)
			if (add_env(shell, arg) != SUCCESS)
				return (ERROR);
		return (SUCCESS);
	}
	return (ERROR);
}
