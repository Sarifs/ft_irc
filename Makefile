NAME = irc_server

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

INCLUDES = -Iinclude

SRC = \
	src/main.cpp \
	src/IRC_Server.cpp \
	src/Client.cpp \
	src/Chanel.cpp \
	src/Parsing.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
