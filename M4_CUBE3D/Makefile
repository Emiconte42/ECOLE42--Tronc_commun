# **************************************************************************** #
#                                   CONFIG                                     #
# **************************************************************************** #

NAME		= cube3d

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -Iincludes

# LDFLAGS		= -lreadline

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

OBJ_DIR = objs

# ===== Source Layout =====
MAIN_DIR = src/
PARSING_DIR = src/0_parsing/

MAIN_FILES = main.c
PARSING_FILES = parsing.c

# ===== Source / Object / Dependency Lists =====
SRCS = \
	$(addprefix $(MAIN_DIR),$(MAIN_FILES)) \
	$(addprefix $(PARSING_DIR),$(PARSING_FILES))

OBJS = $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)

# **************************************************************************** #
#                                 LIBRARIES                                    #
# **************************************************************************** #

MLX_DIR = libs/minilibx-linux
LIBFT_DIR = libs/libft

MLX = $(MLX_DIR)/libmlx.a
LIBFT = $(LIBFT_DIR)/libft.a

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: $(NAME)
	@printf "✔ Compilation OK\n"

$(NAME): $(LIBFT) $(MLX) $(OBJS) 
	@$(CC) $(CFLAGS) $(OBJS) -L$(MLX_DIR) -lmlx -lXext -lX11 -lm $(LIBFT) -o $(NAME)

$(MLX):
	make -C $(MLX_DIR)

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