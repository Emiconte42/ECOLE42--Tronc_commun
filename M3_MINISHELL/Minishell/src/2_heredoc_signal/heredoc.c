/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 11:34:11 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 11:59:01 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	warn_eof(char *delim)
{
	ft_putstr_fd("warning: here-document delimited ", STDERR_FILENO);
	ft_putstr_fd("by end-of-file (wanted `", STDERR_FILENO);
	ft_putstr_fd(delim, STDERR_FILENO);
	ft_putendl_fd("')", STDERR_FILENO);
}

char	*expand_heredoc_line(t_shell *shell, char *line)
{
	t_token	fake_token;

	fake_token.value = ft_gc_strdup(line, &shell->gc_loop);
	fake_token.len_value = ft_strlen(line);
	fake_token.type = TYPE_OTHER;
	fake_token.state = STATE_NORMAL;
	fake_token.next = NULL;
	expand_value(&fake_token, shell);
	return (fake_token.value);
}

int	read_heredoc(t_shell *shell, char *delim, int fd, int in_quote)
{
	char	*line;
	t_sa	sa;

	ft_memset(&sa, 0, sizeof(t_sa));
	sa.sa_handler = ft_handle_heredoc_sigint;
	while (1)
	{
		sigaction(SIGINT, &sa, NULL);
		line = readline("> ");
		if (g_signal == SIGINT || line == NULL)
		{
			if (line == NULL && g_signal != SIGINT)
				warn_eof(delim);
			free(line);
			if (g_signal == SIGINT)
				return (130);
			return (0);
		}
		if (is_delim_line(line, delim))
			return (free(line), SUCCESS);
		if (write_heredoc_line(shell, line, fd, in_quote) != SUCCESS)
			return (ERROR);
	}
}

int	process_heredoc(t_shell *shell)
{
	t_token	*token;

	token = shell->lexer.head;
	while (token)
	{
		if (token->type == TYPE_HEREDOC)
		{
			if (handle_heredoc(shell, token) != SUCCESS)
			{
				close_heredoc_fds(shell);
				return (ERROR);
			}
		}
		token = token->next;
	}
	return (SUCCESS);
}
