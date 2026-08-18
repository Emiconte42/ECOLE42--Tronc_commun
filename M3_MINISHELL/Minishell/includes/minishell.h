/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emiconte <emiconte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 10:41:49 by emiconte          #+#    #+#             */
/*   Updated: 2026/04/24 10:41:49 by emiconte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libs/libft/libft.h"
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <fcntl.h>

# define SIG_INTERACTIVE	0
# define SIG_EXEC			1
# define SIG_HEREDOC		2
# define MS_SIG_DEFAULT		3

extern volatile sig_atomic_t	g_signal;

typedef struct sigaction		t_sa;

typedef enum s_state
{
	STATE_NORMAL		= 0,
	STATE_SIMPLE_QUOTE	= 1,
	STATE_DOUBLE_QUOTE	= 2,
	STATE_DOLLAR_QUOTE	= 3,
}					t_state;

typedef enum s_type
{
	TYPE_SPACE			= 0,
	TYPE_PIPE			= 1,
	TYPE_REDIRECTION	= 2,
	TYPE_SIMPLE_QUOTE	= 3,
	TYPE_DOUBLE_QUOTE	= 4,
	TYPE_OTHER			= 5,
	TYPE_HEREDOC		= 6,
	TYPE_INPUT			= 7,
	TYPE_TRUNCATE		= 8,
	TYPE_APPEND			= 9,
}					t_type;

typedef enum e_error
{
	SUCCESS			= 0,
	ERROR			= 1,
	ERROR_INIT		= 2,
	ERROR_LEXER		= 3,
}					t_error;

typedef struct s_cmd
{
	char			**args;
	char			*path;
	int				fdin;
	int				fdout;
	char			**env;
	int				pid;
	int				perm_denied;
	int				not_found;
	int				is_dir;
	struct s_redir	*redir;
	struct s_cmd	*next;
}					t_cmd;

typedef struct s_redir
{
	char			*file;
	int				type;
	int				heredoc_fd;
	int				in_quote_flag;
	struct s_redir	*next;
}					t_redir;

typedef struct s_gc
{
	void			*content;
	struct s_gc		*next;
}					t_gc;

typedef struct s_expand
{
	char			*var_name;
	char			*var_value;
	int				len_var_value;
	int				len_name;
}					t_expand;

typedef struct s_token
{
	char			*value;
	t_type			type;
	t_state			state;
	int				fd;
	int				len_value;
	char			*value_heredoc;
	int				is_joined;
	struct s_token	*next;
}					t_token;

typedef struct s_lexer
{
	char			*raw;
	int				i;
	int				start;
	int				state_now;
	int				was_joined;
	t_token			*head;
}					t_lexer;

typedef struct s_shell
{
	t_lexer			lexer;
	t_list			*gc_loop;
	t_list			*gc_shell;
	t_list			*gc_env;
	t_expand		expand;
	t_cmd			*cmd;
	int				cmd_list_size;
	char			**env;
	int				exit_status;
	int				stdin;
	int				stdout;
}					t_shell;

/*___________________________________ SRC ___________________________________*/

// UTILS.c
char	**copy_env(char **env, t_shell *shell);
int		is_empty_line(char *line);
int		init_min_env(t_shell *shell, char **argv);
void	handle_exit(t_shell *shell, char *line);

// FREE.c
void	ft_free_line(t_shell *shell, char *line);
void	ft_free_all(t_shell *shell, char *line);
void	clean_gc_history(t_shell *shell);
void	ft_gc_free_env(t_shell *shell);

// CLOSE_FD.c
void	close_heredoc_fds(t_shell *shell);

/*___________________________________ 0_LEXER _______________________________*/

// LEXER.c
int		start_lexer(char *input, t_shell *shell);

// LEXER_INIT.c
int		init_lexer(char *input, t_shell *shell);

// LEXER_UTILS.c
void	handle_normal_space_pipe(t_shell *shell, t_type type, t_lexer *lexer);
void	handle_normal_redirection(t_shell *shell, t_type type, t_lexer *lexer);
void	handle_normal_quotes_other(t_shell *shell, t_type type, t_lexer *lexer);

// LEXER_TOKEN.c
t_type	which_type(char c);
int		close_token(t_shell *shell, t_lexer *lexer);
int		create_token(t_shell *shell, t_lexer *lexer, t_type type);
int		create_token_double_redir(t_shell *shell, t_lexer *lexer, t_type type);

// LEXER_HANDLE.c
int		handle_normal(t_shell *shell, t_lexer *lexer);
int		handle_simple_quote(t_shell *shell, t_lexer *lexer);
int		handle_double_quote(t_shell *shell, t_lexer *lexer);
int		handle_dollar_quote(t_shell *shell, t_lexer *lexer);

/*_________________________________ 1_SYNTAX _________________________________*/

// VALIDATE_SYNTAX.c
int		syntax_error(t_shell *shell, char *msg);
int		validate_syntax(t_shell *shell);

/*_____________________________ 2_HEREDOC_SIGNAL _____________________________*/

// HEREDOC.c
char	*expand_heredoc_line(t_shell *shell, char *line);
int		read_heredoc(t_shell *shell, char *delim, int fd, int in_quote);
int		process_heredoc(t_shell *shell);

// SIGNAL.c
void	ft_handle_heredoc_sigint(int sig);
void	set_signals(int context);

// HEREDOC_FORK.c
int		handle_heredoc(t_shell *shell, t_token *token);

// HEREDOC_UTILS.c
void	ft_disable_main_signal(t_sa *sa_old);
char	*get_heredoc_delim(t_shell *shell, t_token *delim_token);
int		is_delim_line(char *line, char *delim);
int		write_heredoc_line(t_shell *shell, char *line, int fd, int in_quote);
void	close_inherited_heredoc_fds(t_shell *shell, int keep_fd);

/*_________________________________ 3_EXPAND ________________________________*/

// EXPAND.c
int		expand_value(t_token *cur, t_shell *shell);

// EXPAND PREP.c
int		prep_expand(t_shell *shell);

// EXPAND CLEAN.c
char	*ft_str_no_space_tab(char *var_value, t_shell *shell);

// EXPAND UTILS.c
int		is_word_char(char c);
char	*get_env_value(t_shell *shell, t_expand	*expand, char **env);
void	remove_token(t_token *cur, t_token *prev, t_shell *shell);
void	join_skip_chars(t_token *cur, t_shell *shell, int i, int j);
int		check_expand_value(t_shell *shell, t_expand *expand);

/*_____________________________ 4_QUOTE_REMOVAL _____________________________*/

// QUOTE REMOVAL.c
int		quote_removal(t_shell *shell);

/*________________________________ 5_CMD_LIST _______________________________*/

// CMD_LIST.c
int		list_cmd(t_shell *shell);
int		create_cmd(t_shell *shell, t_token **token);
int		init_cmd(t_cmd *cmd, t_shell *shell);
int		assign_cmd(t_cmd *cmd, t_token **token, t_shell *shell);

// CMD_LIST_UTILS.c
int		add_cmd_back(t_shell *shell, t_cmd *new);
int		add_redir_back(t_cmd *cmd, t_redir *new);
int		token_list_size(t_token *token);
void	skip_token_joined(t_token **token);

// PATH.c
char	*find_path(t_shell *shell, t_cmd *cmd);
char	*assign_path(t_shell *shell, t_cmd *cmd);

// PATH_UTILS.c
char	**isolate_path(t_shell *shell, t_cmd *cmd);
int		is_builtin(char *cmd);

// REDIR.c
int		init_redir(t_redir *redir);
int		assign_redir(t_redir *redir, t_token *token, t_shell *shellt);
int		create_redir(t_shell *shell, t_token *token, t_cmd *cmd);

/*_______________________________ 6_BUILTINS _______________________________*/

// 0_BUILTINS.c
int		builtin(t_cmd *cmd, t_shell *shell);

// 1_ECHO.c
int		ft_echo(t_cmd *cmd);

// 2_CD.c
int		ft_cd(t_cmd *cmd, t_shell *shell);

// 2_CD_UTILS.c
int		add_env(t_shell *shell, char *token);
int		update_env_cd(t_shell *shell, const char *key, const char *value);
char	*get_env_var(t_shell *shell, const char *key);

// 3_PWD.c
int		ft_pwd(t_cmd *cmd, t_shell *shell);

// 4_ENV.c
int		ft_env(t_shell *shell);

// 5_EXPORT.c
int		add_env_cd(t_shell *shell, const char *key, const char *value);
int		update_env_export(t_shell *shell, const char *name, char *arg);
int		ft_export(t_cmd *cmd, t_shell *shell);

// 5_EXPORT_UTILS.c
int		is_valid_name(char *str);
int		sort_env(char **env_copy);
int		count_before_equal(char *str);
int		process_arg_with_equal(t_shell *shell, char *arg);

// 6_UNSET.c
int		ft_unset(t_cmd *cmd, t_shell *shell);

// 7_EXIT.c
int		ft_exit(t_cmd *cmd, t_shell *shell);

/*_________________________________ 7_EXEC _________________________________*/

// EXEC.c
int		exec(t_shell *shell);

// EXEC_CHILD.c
void	child(t_cmd *cmd, int fd[2], t_shell **shell);

// EXEC_REDIR.c
int		redir_input(t_shell **shell, t_redir *redir, int *fd);
int		redir_output(t_shell **shell, t_redir *redir, int *fd);
void	close_unused_heredoc_redirs(t_shell *shell, t_cmd *cur_cmd);

// EXEC_HANDLE_REDIR.c
int		handle_redir(t_cmd *cmd, t_shell **shell);

// EXEC_UTILS.c
int		close_and_dup_std(t_shell **shell, int return_value);
int		close_std(t_shell **shell, int return_value);
void	exit_child(t_shell *shell, int exit_code);
void	exit_child_perror(t_shell *shell, char *str, int exit_code);
int		return_redir(t_shell **shell, char *str, int exit_code);

#endif