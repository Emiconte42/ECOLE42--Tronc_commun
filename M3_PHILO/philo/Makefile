# **************************************************************************** #
#                                   CONFIG                                     #
# **************************************************************************** #

MAKEFLAGS	+= -s

NAME		= philo

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -g3
INCLUDES	= -Iincludes

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

SRCS		= srcs/main.c \
			  srcs/utils.c \
			  srcs/clean.c \
			  srcs/parsing.c \
			  srcs/parsing_check.c \
			  srcs/initialisation.c \
			  srcs/start.c \
			  srcs/time.c \
			  srcs/printf.c \
			  srcs/1philo.c \
			  srcs/is_dead.c \
			  srcs/monitor.c \
			  srcs/routine.c \
			  srcs/threads.c \
			  srcs/forks.c \
			  srcs/meals.c

OBJS		  = $(SRCS:.c=.o)

# **************************************************************************** #
#                                   HEADERS                                    #
# **************************************************************************** #

HEADERS		= includes/philo.h \
			  includes/macros.h

# **************************************************************************** #
#                                   COLORS                                     #
# **************************************************************************** #

RESET		= \033[0m
GREEN		= \033[1;32m
WHITE		= \033[1;37m

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: $(NAME)
	@echo "$(GREEN)✔ Compilation OK$(RESET)"

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(NAME) $(OBJS)

%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -f $(OBJS)
	@echo "$(GREEN)✔ Clean OK$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(GREEN)✔ FClean OK$(RESET)"

re: fclean all

.PHONY: all clean fclean re 