# **************************************************************************** #
#                                   CONFIG                                     #
# **************************************************************************** #

NAME		= cub3D

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -Iincludes

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

OBJ_DIR = objs

# ===== Source Layout =====
MAIN_DIR = src/
PARSING_DIR = src/1_parsing/
WINDOW_DIR = src/2_window/
RAYCASTING_DIR = src/3_raycasting/
GAME_DIR = src/4_game/
RENDER_DIR = src/5_render/

MAIN_FILES = main.c
WINDOW_FILES = 1_init_window.c 2_hooks.c 3_mouse.c 4_run.c 5_close.c
PARSING_FILES = 1_extract_data.c 2_parse_params.c 3_1_parse_utils.c 3_2_parse_utils.c 4_map_validate.c 5_map_validate_utils.c 6_map_validate_doors.c
RAYCASTING_FILES = 1_init.c 2_dda.c 3_texture.c 4_texture_load.c 5_render.c
GAME_FILES = 1_movements.c 2_doors.c
RENDER_FILES = 1_render.c 2_render_utils.c 3_minimap.c 4_minimap_utils.c 5_doors.c

# ===== Source / Object / Dependency Lists =====
SRCS = \
	$(addprefix $(MAIN_DIR),$(MAIN_FILES)) \
	$(addprefix $(PARSING_DIR),$(PARSING_FILES)) \
	$(addprefix $(WINDOW_DIR),$(WINDOW_FILES)) \
	$(addprefix $(RAYCASTING_DIR),$(RAYCASTING_FILES)) \
	$(addprefix $(GAME_DIR),$(GAME_FILES)) \
	$(addprefix $(RENDER_DIR),$(RENDER_FILES)) \

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
	$(CC) $(CFLAGS) $(OBJS) -L$(MLX_DIR) -lmlx -lXext -lX11 -lm $(LIBFT) -o $(NAME)

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
