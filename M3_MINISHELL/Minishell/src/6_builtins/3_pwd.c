/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   3_pwd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 17:18:56 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 11:04:08 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	check_pwd_options(t_cmd *cmd, t_shell *shell)
{
	int	i;

	i = 1;
	while (cmd->args[i])
	{
		if (cmd->args[i][0] == '-' && cmd->args[i][1] != '\0'
			&& !(cmd->args[i][1] == '-' && cmd->args[i][2] == '\0'))
		{
			ft_putstr_fd("pwd: ", STDERR_FILENO);
			if (cmd->args[i][1] == '-')
				ft_putstr_fd("--", STDERR_FILENO);
			else
			{
				ft_putstr_fd("-", STDERR_FILENO);
				ft_putchar_fd(cmd->args[i][1], STDERR_FILENO);
			}
			ft_putstr_fd(": invalid option\n", STDERR_FILENO);
			ft_putstr_fd("pwd: usage: pwd [-LP]\n", STDERR_FILENO);
			shell->exit_status = 2;
			return (2);
		}
		i++;
	}
	return (SUCCESS);
}

int	ft_pwd(t_cmd *cmd, t_shell *shell)
{
	char	*buffer;
	int		ret;

	ret = check_pwd_options(cmd, shell);
	if (ret != SUCCESS)
		return (ret);
	buffer = getcwd(NULL, 0);
	if (buffer == NULL)
		return (perror("getcwd"), ERROR);
	ft_putendl_fd(buffer, STDOUT_FILENO);
	free(buffer);
	return (SUCCESS);
}
