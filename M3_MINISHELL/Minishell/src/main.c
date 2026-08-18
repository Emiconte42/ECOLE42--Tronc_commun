/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 19:06:38 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/23 19:06:38 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	run_shell(t_shell *shell, char *line)
{
	if (start_lexer(line, shell) != SUCCESS)
		return (ERROR);
	if (validate_syntax(shell) != SUCCESS)
		return (ERROR);
	if (process_heredoc(shell) != SUCCESS)
		return (ERROR);
	if (prep_expand(shell) != SUCCESS)
		return (ERROR);
	if (quote_removal(shell) != SUCCESS)
		return (ERROR);
	if (list_cmd(shell) != SUCCESS)
		return (ERROR);
	if (exec(shell) != SUCCESS)
		return (ERROR);
	return (SUCCESS);
}

static int	shell_loop(t_shell *shell)
{
	char	*line;

	shell->gc_loop = NULL;
	set_signals(SIG_INTERACTIVE);
	line = readline("🧕 Hissez_les_voiles 👵 > ");
	if (!line)
		handle_exit(shell, line);
	if (is_empty_line(line) != SUCCESS)
		return (free(line), SUCCESS);
	add_history(line);
	if (run_shell(shell, line) != SUCCESS)
	{
		ft_free_line(shell, line);
		g_signal = 0;
		rl_on_new_line();
		rl_replace_line("", 0);
		return (SUCCESS);
	}
	ft_free_all(shell, line);
	return (SUCCESS);
}

int	main(int argc, char **argv, char **env)
{
	t_shell	shell;

	(void)argc;
	ft_bzero(&shell, sizeof(t_shell));
	shell.stdin = -1;
	shell.stdout = -1;
	shell.env = copy_env(env, &shell);
	if (!shell.env)
		return (ERROR);
	if (init_min_env(&shell, argv) != SUCCESS)
	{
		ft_gc_free_env(&shell);
		return (ERROR);
	}
	while (1)
		shell_loop(&shell);
	return (SUCCESS);
}
