# **************************************************************************** #
#                                   CONFIG                                     #
# **************************************************************************** #

NAME = fdf

CC = cc
CFLAGS = -Wall -Wextra -Werror

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

SRCS =	srcs/main.c \
		srcs/get_color.c \
		srcs/clean_and_exit.c \
		srcs/events.c \
		srcs/events_keys_zoom.c \
		srcs/events_keys_move.c \
		srcs/graphics_run.c \
		srcs/graphics_map.c \
		srcs/graphics_line.c \
		srcs/graphics_pixel.c \
		srcs/map_size.c \
		srcs/map_parse.c \
		srcs/initialization.c \
		srcs/map_manager.c

OBJS = $(SRCS:.c=.o)

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

$(NAME): $(OBJS) $(MLX) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) -L$(MLX_DIR) -lmlx -lXext -lX11 -lm $(LIBFT) -o $(NAME)

$(MLX):
	make -C $(MLX_DIR)

$(LIBFT):
	make -C $(LIBFT_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
	make -C $(MLX_DIR) clean
	make -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME)
	make -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
