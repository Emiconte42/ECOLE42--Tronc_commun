/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 15:49:03 by emiconte          #+#    #+#             */
/*   Updated: 2026/06/14 11:54:40 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	ft_disable_main_signal(t_sa *sa_old)
{
	t_sa	sa_new;

	sa_new.sa_handler = SIG_IGN;
	sigemptyset(&sa_new.sa_mask);
	sa_new.sa_flags = 0;
	sigaction(SIGINT, &sa_new, sa_old);
}

char	*get_heredoc_delim(t_shell *shell, t_token *delim_token)
{
	char	*value;
	int		len;

	value = delim_token->value;
	if (value == NULL)
		return (NULL);
	len = ft_strlen(value);
	if (len >= 2 && ((value[0] == '\'' && value[len - 1] == '\'')
			|| (value[0] == '"' && value[len - 1] == '"')))
		return (ft_gc_substr(value, 1, len - 2, &shell->gc_loop));
	return (ft_gc_strdup(value, &shell->gc_loop));
}

int	is_delim_line(char *line, char *delim)
{
	int	line_len;
	int	delim_len;

	if (line == NULL || delim == NULL)
		return (0);
	line_len = ft_strlen(line);
	if (line_len > 0 && line[line_len - 1] == '\n')
		line_len--;
	delim_len = ft_strlen(delim);
	if (line_len != delim_len)
		return (0);
	return (ft_strncmp(line, delim, delim_len) == 0);
}

int	write_heredoc_line(t_shell *shell, char *line, int fd, int in_quote)
{
	char	*expanded;
	int		ret;

	if (!in_quote)
		expanded = expand_heredoc_line(shell, line);
	else
		expanded = line;
	ret = write(fd, expanded, ft_strlen(expanded));
	free(line);
	if (ret < 0)
		return (ERROR);
	ret = write(fd, "\n", 1);
	if (ret < 0)
		return (ERROR);
	return (SUCCESS);
}

void	close_inherited_heredoc_fds(t_shell *shell, int keep_fd)
{
	t_token	*tok;

	tok = shell->lexer.head;
	while (tok)
	{
		if (tok->type == TYPE_HEREDOC && tok->fd > 0 && tok->fd != keep_fd)
		{
			close(tok->fd);
			tok->fd = -1;
		}
		tok = tok->next;
	}
}
