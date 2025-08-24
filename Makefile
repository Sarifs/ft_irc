# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: idioumas <idioumas@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/24 16:44:35 by idioumas          #+#    #+#              #
#    Updated: 2025/08/24 16:51:13 by idioumas         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Nom de l'exécutable
NAME = irc

# Compilateur et options
CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -Iinclude

# Sources (main + src/*.cpp)
SRCS = main.cpp \
       src/Client.cpp \
       src/parsing.cpp

# Objets correspondants
OBJS = $(SRCS:.cpp=.o)

# Cible par défaut
all: $(NAME)

# Link final
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Compilation des .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des .o
clean:
	rm -f $(OBJS)

# Nettoyage complet
fclean: clean
	rm -f $(NAME)

# Recompilation totale
re: fclean all

# Lancer directement le programme
run: all
	./$(NAME)

.PHONY: all clean fclean re run
