# **************************************************************************** #
#                                   CONFIG                                     #
# **************************************************************************** #

NAME		= interface

CC			= c++

CFLAGS		= -Wall -Wextra -Werror -std=c++98

INCLUDES	= -I includes -I utils

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

OBJ_DIR = objs/

SRCS =		main.cpp \
			srcs/AMateria.cpp \
			srcs/Character.cpp \
			srcs/Cure.cpp \
			srcs/Ice.cpp \
			srcs/MateriaSource.cpp \
			utils/utils.cpp

HEADERS =	includes/AMateria.hpp \
			includes/Character.hpp \
			includes/Cure.hpp \
			includes/Ice.hpp \
			includes/ICharacter.hpp \
			includes/IMateriaSource.hpp \
			includes/MateriaSource.hpp \
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