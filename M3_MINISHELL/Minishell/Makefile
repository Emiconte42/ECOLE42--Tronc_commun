# **************************************************************************** #
#                                   CONFIG                                     #
# **************************************************************************** #

NAME		= minishell

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -Iincludes

LDFLAGS		= -lreadline

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

OBJ_DIR = objs

# ===== Source Layout =====
MAIN_DIR = src/
LEXER_DIR = src/0_lexer/
VALIDATE_DIR = src/1_syntax/
HEREDOC_DIR = src/2_heredoc_signal/
EXPAND_DIR = src/3_expand/
QUOTE_REMOVAL_DIR = src/4_quote_removal/
CMD_LIST_DIR = src/5_cmd_list/
BUILTIN_DIR = src/6_builtins/
EXEC_DIR = src/7_exec/

MAIN_FILES = main.c utils.c free.c close_fd.c
LEXER_FILES = lexer.c lexer_handle.c lexer_init.c lexer_utils.c lexer_token.c
VALIDATE_FILES = validate_syntax.c
HEREDOC_FILES = heredoc.c heredoc_utils.c heredoc_fork.c signal.c
EXPAND_FILES = expand.c expand_utils.c expand_clean.c expand_prep.c
QUOTE_REMOVAL_FILES = quote_removal.c
CMD_LIST_FILES = cmd_list.c cmd_list_utils.c redir.c path.c path_utils.c
BUILTIN_FILES = 0_builtins.c 1_echo.c 2_cd.c 2_cd_utils.c 3_pwd.c 4_env.c 5_export.c 5_export_utils.c 6_unset.c 7_exit.c
EXEC_FILES = exec.c exec_utils.c exec_redir.c exec_child.c exec_handle_redir.c

# ===== Source / Object / Dependency Lists =====
SRCS = \
	$(addprefix $(MAIN_DIR),$(MAIN_FILES)) \
	$(addprefix $(LEXER_DIR),$(LEXER_FILES)) \
	$(addprefix $(VALIDATE_DIR),$(VALIDATE_FILES)) \
	$(addprefix $(EXPAND_DIR),$(EXPAND_FILES)) \
	$(addprefix $(HEREDOC_DIR),$(HEREDOC_FILES)) \
	$(addprefix $(QUOTE_REMOVAL_DIR),$(QUOTE_REMOVAL_FILES)) \
	$(addprefix $(CMD_LIST_DIR),$(CMD_LIST_FILES)) \
	$(addprefix $(BUILTIN_DIR),$(BUILTIN_FILES)) \
	$(addprefix $(EXEC_DIR),$(EXEC_FILES))

OBJS = $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)

# **************************************************************************** #
#                                 LIBRARIES                                    #
# **************************************************************************** #

LIBFT_DIR = libs/libft
LIBFT = $(LIBFT_DIR)/libft.a

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: $(NAME)
	@printf "✔ Compilation OK\n"

$(NAME): $(LIBFT) $(OBJS) 
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(LDFLAGS) -o $(NAME)

$(LIBFT):
	make -C $(LIBFT_DIR)

$(OBJ_DIR)/%.o:  %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	make -C $(LIBFT_DIR) clean
	@printf "✔ Clean OK\n"

fclean: clean
	@rm -f $(NAME)
	make -C $(LIBFT_DIR) fclean
	@printf "✔ FClean OK\n"

re: fclean all

.PHONY: all clean fclean re