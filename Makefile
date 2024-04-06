# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jolecomt <jolecomt@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/21 11:50:48 by jolecomt          #+#    #+#              #
#    Updated: 2024/03/28 17:16:08 by jolecomt         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME          = ircserv
INCLUDES      = inc/
SRC_DIR       = src/
OBJ_DIR       = obj/

CXX           = c++
CXXFLAGS      = -Wall -Wextra -Werror -std=c++98

SRC_FILES     = main\
				socketClient\
				socketServer\
				utils


SRC           = $(addprefix $(SRC_DIR), $(addsuffix .cpp, $(SRC_FILES)))
OBJ           = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC_FILES)))

OBJ_CACHE     = .cache_exists

#####

all:			$(NAME)

$(NAME):		$(OBJ)
					$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
					@echo "$(NAME) compiled!"


$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp | $(OBJ_CACHE)
					@echo "Compiling $<"
					@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_CACHE):
					@mkdir -p $(OBJ_DIR)

clean:
					@rm -rf $(OBJ_DIR)
					@echo "$(NAME) object files cleaned!"

fclean:			clean
					@rm -f $(NAME)
					@echo "$(NAME) executable files cleaned!"

re:				fclean all
					@echo "Cleaned and rebuilt everything for $(NAME)!"

.PHONY:			all clean fclean re