/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_fork.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 09:47:07 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 11:19:50 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	heredoc_child(t_shell *shell, int *pipe_fd, char *delim,
				int in_quote)
{
	int	status;

	close(pipe_fd[0]);
	close_inherited_heredoc_fds(shell, pipe_fd[1]);
	status = read_heredoc(shell, delim, pipe_fd[1], in_quote);
	close(pipe_fd[1]);
	exit_child(shell, status);
}

static int	heredoc_parent(t_shell *shell, int *pipe_fd,
				pid_t pid, t_sa *sa_old)
{
	int	status;

	waitpid(pid, &status, 0);
	close(pipe_fd[1]);
	if (WIFSIGNALED(status)
		|| (WIFEXITED(status) && WEXITSTATUS(status) == 130))
	{
		close(pipe_fd[0]);
		sigaction(SIGINT, sa_old, NULL);
		shell->exit_status = 130;
		return (ERROR);
	}
	return (SUCCESS);
}

static int	init_heredoc(t_shell *shell, t_token *token, char **delim,
				int *in_quote)
{
	*in_quote = (token->next->state == STATE_SIMPLE_QUOTE
			|| token->next->state == STATE_DOUBLE_QUOTE);
	*delim = get_heredoc_delim(shell, token->next);
	if (*delim == NULL)
		return (ERROR);
	return (SUCCESS);
}

int	handle_heredoc(t_shell *shell, t_token *token)
{
	char	*delim;
	int		pipe_fd[2];
	int		in_quote;
	pid_t	pid;
	t_sa	sa_old;

	ft_disable_main_signal(&sa_old);
	if (token->next == NULL)
		return (ERROR);
	if (init_heredoc(shell, token, &delim, &in_quote) != SUCCESS)
		return (ERROR);
	if (pipe(pipe_fd) == -1)
		return (perror("pipe"), ERROR);
	pid = fork();
	if (pid == -1)
		return (perror("fork"), close(pipe_fd[0]), close(pipe_fd[1]), ERROR);
	if (pid == 0)
		heredoc_child(shell, pipe_fd, delim, in_quote);
	if (heredoc_parent(shell, pipe_fd, pid, &sa_old) != SUCCESS)
		return (ERROR);
	token->fd = pipe_fd[0];
	sigaction(SIGINT, &sa_old, NULL);
	return (SUCCESS);
}
