/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 19:29:33 by wihumeau          #+#    #+#             */
/*   Updated: 2026/06/14 11:00:50 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	ft_free_line(t_shell *shell, char *line)
{
	free(line);
	close_heredoc_fds(shell);
	ft_gc_free(&shell->gc_loop);
	shell->lexer.head = NULL;
	shell->cmd = NULL;
}

void	ft_free_all(t_shell *shell, char *line)
{
	free(line);
	close_heredoc_fds(shell);
	ft_gc_free(&shell->gc_loop);
	ft_gc_free(&shell->gc_shell);
	shell->lexer.head = NULL;
	shell->cmd = NULL;
}

void	clean_gc_history(t_shell *shell)
{
	ft_gc_free(&shell->gc_loop);
	ft_gc_free(&shell->gc_shell);
	ft_gc_free_env(shell);
	shell->env = NULL;
	clear_history();
}

void	ft_gc_free_env(t_shell *shell)
{
	ft_gc_free(&shell->gc_env);
	shell->env = NULL;
}
