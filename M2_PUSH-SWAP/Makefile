# **************************************************************************** #
#                                   CONFIG                                     #
# **************************************************************************** #

MAKEFLAGS   += -s

NAME        = push_swap

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -g3

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

SRCS        = srcs/main.c \
              srcs/parsing.c \
			  srcs/utils_list.c \
			  srcs/utils_triage.c \
			  srcs/initialisation.c \
			  srcs/mouv_push.c \
			  srcs/mouv_swap.c \
			  srcs/mouv_rotation.c \
			  srcs/mouv_rotation_rev.c \
			  srcs/operation.c \
			  srcs/algo.c \
			  srcs/radix.c \
              srcs/free.c

OBJS        = $(SRCS:.c=.o)

# **************************************************************************** #
#                                   HEADERS                                    #
# **************************************************************************** #

HEADERS     = includes/*.h

# **************************************************************************** #
#                                   LIBFT                                      #
# **************************************************************************** #

LIBFT_DIR   = libs/libft
LIBFT       = $(LIBFT_DIR)/libft.a

# **************************************************************************** #
#                                   COLORS                                     #
# **************************************************************************** #

RESET   = \033[0m
GREEN   = \033[1;32m
WHITE   = \033[1;37m

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: $(NAME)
	@echo "$(GREEN)✔ Compilation OK$(RESET)"

$(NAME): $(OBJS) $(LIBFT)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFT)

$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)
	@$(MAKE) -C $(LIBFT_DIR) clean
	@echo "$(GREEN)✔ Clean OK$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@echo "$(GREEN)✔ FClean OK$(RESET)"

re: fclean all

.PHONY: all clean fclean re 