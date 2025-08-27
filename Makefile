# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: idioumas <idioumas@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/24 16:44:35 by idioumas          #+#    #+#              #
#    Updated: 2025/08/27 17:28:20 by idioumas         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Nom de l'exécutable
NAME = irc_server

# Compilateur et options
CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -Iinclude

# Sources
SRCS = main.cpp \
       src/Client.cpp \

# Objets générés
OBJS = $(SRCS:.cpp=.o)

# Règle principale
all: $(NAME)

# Édition de liens
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Compilation des .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage des objets
clean:
	rm -f $(OBJS)

# Nettoyage complet
fclean: clean
	rm -f $(NAME)

# Recompilation complète
re: fclean all

# Lancer directement
run: all
	./$(NAME)

.PHONY: all clean fclean re run

