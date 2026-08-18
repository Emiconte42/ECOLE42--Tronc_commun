/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   1_echo.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 16:44:40 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 09:33:49 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_valid_n_flag(char *value)
{
	int	j;

	if (!value || value[0] != '-')
		return (0);
	j = 1;
	while (value[j])
	{
		if (value[j] != 'n')
			return (0);
		j++;
	}
	return (j > 1);
}

int	ft_echo(t_cmd *cmd)
{
	int		newline;
	int		i;
	int		first_arg;

	newline = 1;
	i = 1;
	while (cmd->args[i] && is_valid_n_flag(cmd->args[i]) == 1)
	{
		newline = 0;
		i++;
	}
	first_arg = 1;
	while (cmd->args[i])
	{
		if (!first_arg)
			write(1, " ", 1);
		write(1, cmd->args[i], ft_strlen(cmd->args[i]));
		first_arg = 0;
		i++;
	}
	if (newline)
		write(1, "\n", 1);
	return (SUCCESS);
}
