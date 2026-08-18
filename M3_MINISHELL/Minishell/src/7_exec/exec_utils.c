/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:44:58 by wihumeau          #+#    #+#             */
/*   Updated: 2026/06/14 12:10:04 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	close_and_dup_std(t_shell **shell, int return_value)
{
	if ((*shell)->stdout != -1)
		dup2((*shell)->stdout, STDOUT_FILENO);
	if ((*shell)->stdin != -1)
		dup2((*shell)->stdin, STDIN_FILENO);
	close_std(shell, return_value);
	return (return_value);
}

int	close_std(t_shell **shell, int return_value)
{
	if ((*shell)->stdin != -1)
	{
		close((*shell)->stdin);
		(*shell)->stdin = -1;
	}
	if ((*shell)->stdout != -1)
	{
		close((*shell)->stdout);
		(*shell)->stdout = -1;
	}
	return (return_value);
}

void	exit_child(t_shell *shell, int exit_code)
{
	close_heredoc_fds(shell);
	ft_gc_free(&shell->gc_shell);
	ft_gc_free(&shell->gc_loop);
	ft_gc_free_env(shell);
	clear_history();
	exit(exit_code);
}

void	exit_child_perror(t_shell *shell, char *str, int exit_code)
{
	perror(str);
	close_heredoc_fds(shell);
	ft_gc_free(&shell->gc_shell);
	ft_gc_free(&shell->gc_loop);
	ft_gc_free_env(shell);
	clear_history();
	exit(exit_code);
}

int	return_redir(t_shell **shell, char *str, int exit_code)
{
	perror(str);
	(*shell)->exit_status = exit_code;
	return (ERROR);
}
