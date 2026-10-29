# **************************************************************************** #
#                                   CONFIG                                     #
# **************************************************************************** #

NAME		= brain

CC			= c++

CFLAGS		= -Wall -Wextra -Werror -std=c++98

INCLUDES	= -I includes -I utils

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

OBJ_DIR = objs/

SRCS =		main.cpp \
			srcs/Brain.cpp \
			srcs/Animal.cpp \
			srcs/Cat.cpp \
			srcs/Dog.cpp \
			srcs/WrongAnimal.cpp \
			srcs/WrongCat.cpp \
			utils/utils.cpp

HEADERS =	includes/Animal.hpp \
			includes/Brain.hpp \
			includes/Cat.hpp \
			includes/Dog.hpp \
			includes/WrongAnimal.hpp \
			includes/WrongCat.hpp \
			utils/utils.hpp

# **************************************************************************** #
#                                   COMPIL                                     #
# **************************************************************************** #

OBJS = $(SRCS:%.cpp=$(OBJ_DIR)%.o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)%.o : %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean :
	rm -rf $(OBJ_DIR)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY : clean fclean all re